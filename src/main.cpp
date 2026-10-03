#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>

struct Telemetry
{
    double voltage_volts;
    double current_amps;
};

struct TelemetryLimits
{
    double min_vol;
    double max_vol;
    double max_current;
};


enum class HealthStatus
{
    healthy,
    under_voltage,
    over_voltage,
    over_current,
};

enum class PacketStatus
{
    valid,
    invalid_size,
    invalid_start_byte,
    invalid_message_id,
    invalid_payload_length,
    invalid_checksum,
};

PacketStatus validate_packet(const std::vector<std::uint8_t> & packet)
{
    constexpr std::size_t expected_size{8};

    if (packet.size() != expected_size) {
        return PacketStatus::invalid_size;
    }

    if (packet[0] != 0xAA) {
        return PacketStatus::invalid_start_byte;
    }

    if (packet[1] != 0x01) {
        return PacketStatus::invalid_message_id;
    }

    if (packet[2] != 0x04) {
        return PacketStatus::invalid_payload_length;
    }

    std::uint8_t cal_check{0};
    for(std::size_t index{0}; index < packet.size()-1; ++index)
    {
        cal_check ^= packet[index];
    }
    if(cal_check!=packet.back())
    {
        return PacketStatus::invalid_checksum;
    }
    return PacketStatus::valid;
}

void print_error_status(PacketStatus status)
{
    switch(status){
        case PacketStatus::valid:
        return;

        case PacketStatus::invalid_size:
        std::cerr<<"Invalid size\n";
        return;

        case PacketStatus::invalid_start_byte:
        std::cerr<<"Invalid start byte\n";
        return;

        case PacketStatus::invalid_message_id:
        std::cerr<<"Invalid message ID\n";
        return;

        case PacketStatus::invalid_payload_length:
        std::cerr<<"Invalid payload length\n";
        return;

        case PacketStatus::invalid_checksum:
        std::cerr<<"Invalid checksum\n";
        return;

    };
}

Telemetry decode_tele(const std::vector<std::uint8_t> &packet)
{
    Telemetry tele;
    auto low_v= packet[3];
    auto high_v=packet[4];
    auto low_c=packet[5];
    auto high_c=packet[6];

    auto total_v= (high_v<<8)+low_v;
    auto total_c=(high_c<<8)+low_c;

    tele.voltage_volts=total_v/1000.0;
    tele.current_amps=total_c/1000.0;

    return tele;
}
HealthStatus evaluate_health(const Telemetry &tele, const TelemetryLimits &lims)
{
    if(tele.voltage_volts<lims.min_vol)
    {
        return HealthStatus::under_voltage;
    }
    if(tele.voltage_volts>lims.max_vol)
    {
        return HealthStatus::over_voltage;
    }
    if(tele.current_amps>lims.max_current)
    {
        return HealthStatus::over_current;
    }

    return HealthStatus::healthy;
}

void process_packet(const std::vector<std::uint8_t> &packet, const TelemetryLimits & limits)
{
    const PacketStatus packet_status= validate_packet(packet);
    if(packet_status!=PacketStatus::valid)
    {
        print_error_status(packet_status);
        return;
    }
    auto telem=decode_tele(packet);
    std::cout<<"Total voltage:"<<telem.voltage_volts<< "v\n"<< "Total current:"<<telem.current_amps<<"amp\n";
    auto s=evaluate_health(telem, limits);
    switch(s){
            case HealthStatus::healthy:
            std::cout<<"healthy\n";
            break;
            case HealthStatus::over_current:
            std::cout<<"over_current\n";
            break;
            case HealthStatus::over_voltage:
            std::cout<<"over_voltage\n";
            break;
            case HealthStatus::under_voltage:
            std::cout<<"under_voltage\n";
            break;
    }
}


int main (){

    const std::vector<std::uint8_t> incoming_bytes{
    0x00, 0x55,                         // noise

    0xAA, 0x01, 0x04,                   // packet 1 begins
    0xE0, 0x5E, 0xDC, 0x05, 0xB8,

    0x13,                               // noise

    0xAA, 0x01, 0x04,                   // packet 2 begins
    0xE0, 0x2E, 0xC4, 0x09, 0xAC
    };

    const TelemetryLimits safe_limits{11.0, 15.6, 2.0};
    std::vector<std::uint8_t> receive_buffer;
    for (const auto  byte: incoming_bytes){
        if(receive_buffer.empty()&&byte!=0xAA){
            continue;
        }
        receive_buffer.push_back(byte);
        if(receive_buffer.size()<8)
        {
            continue;
        }
    process_packet(receive_buffer,safe_limits);

    receive_buffer.clear();
    }

    return 0;
}
