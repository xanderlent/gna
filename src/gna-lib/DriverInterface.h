/**
 @copyright Copyright (C) 2017-2022 Intel Corporation
 SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include "Request.h"

#include "gna2-common-impl.h"

namespace GNA
{

class HardwareRequest;

struct HardwarePerfResults
{
    uint64_t total; // # of total cycles spent on scoring in hw
    uint64_t stall; // # of stall cycles spent in hw (since scoring)
};

struct DriverPerfResults
{
    /**
     Request preprocessing start
     */
    uint64_t Preprocessing;

    /**
     Request processing started by hardware
     */
    uint64_t Processing;

    /**
     Request completed interrupt triggered by hardware
     */
    uint64_t DeviceRequestCompleted;

    /**
     Driver completed interrupt and request handling.
     */
    uint64_t Completion;
};

struct RequestResult
{
    HardwarePerfResults hardwarePerf;
    DriverPerfResults driverPerf;
    Gna2Status status;
};

enum GnaIoctlCommand
{
    GNA_COMMAND_MAP,
    GNA_COMMAND_UNMAP,
    GNA_COMMAND_SCORE,
    GNA_COMMAND_GET_PARAM,
};

struct DriverCapabilities
{
    uint32_t hwInBuffSize;
    uint32_t recoveryTimeout;
    DeviceVersion deviceVersion;

    /**
     Number of ticks of driver performance counter per second.
     */
    uint64_t perfCounterFrequency;

    bool isSoftwareFallbackSupported;
};

class DriverInterface
{
public:
    DriverInterface(const DriverInterface&) = delete;
    DriverInterface(DriverInterface&&) = delete;
    DriverInterface& operator=(const DriverInterface&) = delete;
    DriverInterface& operator=(DriverInterface&&) = delete;

    static DeviceVersion Query(uint32_t deviceIndex);

    // unique_ptr is guaranteed to be not null
    static std::unique_ptr<DriverInterface> Create(uint32_t deviceIndex);

    static constexpr uint8_t MAX_GNA_DEVICES = 16;

    virtual bool OpenDevice(uint32_t deviceIndex) = 0;

    virtual ~DriverInterface() = default;

    const DriverCapabilities& GetCapabilities() const;

    virtual uint64_t MemoryMap(void *memory, uint32_t memorySize) = 0;

    virtual void MemoryUnmap(uint64_t memoryId) = 0;

    virtual RequestResult Submit(
        HardwareRequest& hardwareRequest, RequestProfiler & profiler) const = 0;

protected:
    DriverInterface() = default;

    virtual void createRequestDescriptor(HardwareRequest& hardwareRequest) const = 0;

    virtual Gna2Status parseHwStatus(uint32_t hwStatus) const = 0;

    void convertPerfResultUnit(DriverPerfResults & driverPerf,
        Gna2InstrumentationUnit targetUnit) const;

    void convertPerfResultUnit(HardwarePerfResults & hardwarePerf,
        const Gna2InstrumentationUnit targetUnit) const;

    static void convertPerfResultUnit(DriverPerfResults & driverPerf,
        uint64_t frequency, uint64_t multiplier);

    static void convertPerfResultUnit(HardwarePerfResults & hardwarePerf,
        const uint64_t frequency, const uint64_t multiplier);

    DriverCapabilities driverCapabilities = {};
};

}
