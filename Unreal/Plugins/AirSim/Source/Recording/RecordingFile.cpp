#include "RecordingFile.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include <sstream>
#include "ImageUtils.h"
#include "common/ClockFactory.hpp"
#include "common/common_utils/FileSystem.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>

void RecordingFile::appendRecord(const std::vector<msr::airlib::ImageCaptureBase::ImageResponse>& responses,
                                 msr::airlib::VehicleSimApiBase* vehicle_sim_api)
{
    bool save_success = false;
    std::ostringstream image_file_names;
    std::unique_ptr<std::future<void>> opt;

    for (auto i = 0; i < responses.size(); ++i) {
        auto time_a = std::chrono::high_resolution_clock::now();

        const auto& response = responses.at(i);

        //build image file name
        std::ostringstream image_file_name;
        image_file_name << "img_"
                        << vehicle_sim_api->getVehicleName() << "_"
                        << response.camera_name << "_" << common_utils::Utils::toNumeric(response.image_type) << "_" << common_utils::Utils::getTimeSinceEpochNanos();
        std::string extension;
        if (response.pixels_as_float)
            extension = ".pfm";
        else if (response.compress)
            extension = ".png";
        else {
            //extension = ".ppm";
            extension = ".jpg";
        }

        image_file_name << extension;

        if (i > 0)
            image_file_names << ";";
        image_file_names << image_file_name.str();
        std::string image_full_file_path = common_utils::FileSystem::combine(image_path_, image_file_name.str());


        //write image file
        try {
            if (extension == ".pfm") {
                common_utils::Utils::writePFMfile(response.image_data_float.data(), response.width, response.height, image_full_file_path);
            }
            else if (extension == ".ppm" || extension == ".jpg") {
                std::lock_guard<std::mutex> lock(task_mutex);

                if (extension == ".ppm") {
                    tasks.push([response, image_full_file_path] {
                        // Here goes the actual file writing logic, e.g.:
                        common_utils::Utils::writePPMfile(response.image_data_uint8.data(), response.width, response.height, image_full_file_path);
                        //std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulating work
                        });
                }
                else {
                    tasks.push([response, image_full_file_path] {
                        // Here goes the actual file writing logic, e.g.:
                        common_utils::Utils::writePPMToJPGfile(response.image_data_uint8.data(), response.width, response.height, image_full_file_path);
                        //std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulating work
                        });
                }

                tasks.push([response, image_full_file_path] {
                    // Here goes the actual file writing logic, e.g.:

                        common_utils::Utils::writePPMfile(response.image_data_uint8.data(), response.width, response.height, image_full_file_path);
                        common_utils::Utils::writePPMToJPGfile(response.image_data_uint8.data(), response.width, response.height, image_full_file_path);
                    //std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulating work
                    });

                // Notify the worker thread that there is a new task
                task_condition.notify_one();
                
                //std::future<void> futurePPM = std::async(std::launch::async, [response, image_full_file_path]() {
                //    //common_utils::Utils::writePPMfile(response.image_data_uint8.data(), response.width, response.height, image_full_file_path);
                //    // this thread sleep for 1 second
                //    std::this_thread::sleep_for(std::chrono::seconds(1));
                //    return;
                //    });
                //opt = std::make_unique<std::future<void>>(std::move(futurePPM));
               
            }
            else {
                // Write PNG image, already compressed in binary
                std::ofstream file(image_full_file_path, std::ios::binary);
                file.write(reinterpret_cast<const char*>(response.image_data_uint8.data()), response.image_data_uint8.size());
                file.close();
            }

            save_success = true;
        }
        catch (std::exception& ex) {
            save_success = false;
            UAirBlueprintLib::LogMessage(TEXT("Image file save failed"), FString(ex.what()), LogDebugLevel::Failure);
        }
    }

    //write to CSV file
    if (save_success || (responses.size() == 0)) {
        // Either images were saved successfully, or there were no images
        writeString(vehicle_sim_api->getRecordFileLine(false).append(image_file_names.str()).append("\n"));

        //UAirBlueprintLib::LogMessage(TEXT("Screenshot saved to:"), filePath, LogDebugLevel::Success);
    }
    return;
}

void RecordingFile::appendColumnHeader(const std::string& header_columns)
{
    writeString(header_columns + "ImageFile" + "\n");
}

void RecordingFile::createFile(const std::string& file_path, const std::string& header_columns)
{
    try {
        closeFile();

        IPlatformFile& platform_file = FPlatformFileManager::Get().GetPlatformFile();
        log_file_handle_ = platform_file.OpenWrite(*FString(file_path.c_str()));
        appendColumnHeader(header_columns);
    }
    catch (std::exception& ex) {
        UAirBlueprintLib::LogMessageString(std::string("createFile Failed for ") + file_path, ex.what(), LogDebugLevel::Failure);
    }
}

bool RecordingFile::isFileOpen() const
{
    return log_file_handle_ != nullptr;
}

void RecordingFile::closeFile()
{
    if (isFileOpen())
        delete log_file_handle_;

    log_file_handle_ = nullptr;
}

void RecordingFile::writeString(const std::string& str) const
{
    try {
        if (log_file_handle_) {
            FString line_f(str.c_str());
            log_file_handle_->Write((const uint8*)TCHAR_TO_ANSI(*line_f), line_f.Len());
        }
        else
            UAirBlueprintLib::LogMessageString("Attempt to write to recording log file when file was not opened", "", LogDebugLevel::Failure);
    }
    catch (std::exception& ex) {
        UAirBlueprintLib::LogMessageString(std::string("file write to recording file failed "), ex.what(), LogDebugLevel::Failure);
    }
}

RecordingFile::RecordingFile() : stop_worker(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] { processTasks(); });
    }
}

RecordingFile::~RecordingFile()
{
    stopRecording(true);
    // Signal the worker to stop and join the thread
    {
        std::lock_guard<std::mutex> lock(task_mutex);
        stop_worker = true;
    }
    task_condition.notify_all();
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void RecordingFile::startRecording(msr::airlib::VehicleSimApiBase* vehicle_sim_api, const std::string& folder)
{
    try {
        std::string log_folderpath = common_utils::FileSystem::getLogFolderPath(true, folder);
        image_path_ = common_utils::FileSystem::ensureFolder(log_folderpath, "images");
        std::string log_filepath = common_utils::FileSystem::getLogFileNamePath(log_folderpath, record_filename, "", ".txt", false);
        if (log_filepath != "")
            createFile(log_filepath, vehicle_sim_api->getRecordFileLine(true));
        else {
            UAirBlueprintLib::LogMessageString("Cannot start recording because path for log file is not available", "", LogDebugLevel::Failure);
            return;
        }

        if (isFileOpen()) {
            is_recording_ = true;

            UAirBlueprintLib::LogMessage(TEXT("Recording: "), TEXT("Started"), LogDebugLevel::Success);
        }
        else
            UAirBlueprintLib::LogMessageString("Error creating log file", log_filepath.c_str(), LogDebugLevel::Failure);
    }
    catch (const std::invalid_argument& e) {
        // Log the exception type and what it says
        UAirBlueprintLib::LogMessageString("Invalid argument in startRecording: ", e.what(), LogDebugLevel::Failure);
        UE_LOG(LogTemp, Error, TEXT("Invalid argument in startRecording: %s"), *FString(e.what()));
    }
    catch (...) {
        UAirBlueprintLib::LogMessageString("Error in startRecording", "", LogDebugLevel::Failure);
    }
}

void RecordingFile::stopRecording(bool ignore_if_stopped)
{
    is_recording_ = false;
    if (!isFileOpen()) {
        if (ignore_if_stopped)
            return;

        UAirBlueprintLib::LogMessage(TEXT("Recording Error"), TEXT("File was not open"), LogDebugLevel::Failure);
    }
    else
        closeFile();

    UAirBlueprintLib::LogMessage(TEXT("Recording: "), TEXT("Stopped"), LogDebugLevel::Success);
    UAirBlueprintLib::LogMessage(TEXT("Data saved to: "), FString(image_path_.c_str()), LogDebugLevel::Success);
}

bool RecordingFile::isRecording() const
{
    return is_recording_;
}


void RecordingFile::processTasks() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(task_mutex);
            task_condition.wait(lock, [this] { return stop_worker || !tasks.empty(); });
            if (stop_worker && tasks.empty()) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }

        // Execute the task outside of the lock to allow new tasks to be enqueued
        task();
    }
}