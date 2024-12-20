#pragma once

#include "CoreMinimal.h"
#include <string>
#include "AirBlueprintLib.h"
#include "physics/Kinematics.hpp"
#include "HAL/FileManager.h"
#include "PawnSimApi.h"
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>



class RecordingFile
{
public:
	RecordingFile();
    ~RecordingFile();

    void appendRecord(const std::vector<msr::airlib::ImageCaptureBase::ImageResponse>& responses, msr::airlib::VehicleSimApiBase* vehicle_sim_api);
    void appendColumnHeader(const std::string& header_columns);
    void startRecording(msr::airlib::VehicleSimApiBase* vehicle_sim_api, const std::string& folder = "");
    void stopRecording(bool ignore_if_stopped);
    bool isRecording() const;

    void processTasks();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex task_mutex;
    std::condition_variable task_condition;
    bool stop_worker = false;
    int threads = 16;

    void createFile(const std::string& file_path, const std::string& header_columns);
    void closeFile();
    void writeString(const std::string& line) const;
    bool isFileOpen() const;

    std::string record_filename = "airsim_rec";
    std::string image_path_;
    bool is_recording_ = false;
    IFileHandle* log_file_handle_ = nullptr;

    
};