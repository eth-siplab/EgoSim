#pragma once

#include "CoreMinimal.h"
#include "SIPCharacterPawn.h"
#include "PawnEvents.h"
#include "PawnSimApi.h"
#include "common/Common.hpp"
#include "common/CommonStructs.hpp"

class SIPCharacterPawnSimApi : public PawnSimApi
{

private:
    ASIPCharacterPawn* sip_pawn_;
    const uint64_t create_time;

public:
    //std::chrono::high_resolution_clock sip_char_clock;
    virtual void initialize() override;
    virtual ~SIPCharacterPawnSimApi() = default;
    SIPCharacterPawnSimApi(const Params& params);
    virtual std::string getRecordFileLine(bool is_header_line) const override;
};