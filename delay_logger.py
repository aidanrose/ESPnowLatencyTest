import serial
import csv
import time
import argparse
from pathlib import Path

def extract_response_delay(line: bytes) -> int | str:
    try:
        return int(line.decode())
    except ValueError:
        return 'No Data'

    

def write_to_file(output_path: Path, data_buffer: list[tuple[float, ...]]) -> None:
    with open(output_path, 'a') as file:
        csv_writer = csv.writer(file, lineterminator='\n')
        csv_writer.writerows(data_buffer)

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("device", type=str)
    parser.add_argument("-b", "--baud", type=int, default=115200)
    parser.add_argument("-o", "--output", type=str, default="data")
    parser.add_argument("-n", "--max-buffer", type=int, default=50)
    args = parser.parse_args()

    device_path = args.device
    baud = args.baud
    output_dir = Path(args.output)
    max_buffer = args.max_buffer

    device = serial.Serial(device_path, baudrate=baud)
    output_dir.mkdir(exist_ok=True)
    output_path = output_dir / f"{time.time()}.csv"
    data_buffer = []

    try:
        while True:
            line = device.readline()
            print(line)
            receive_time = time.time()
            response_delay = extract_response_delay(line)
            data_buffer.append((receive_time, response_delay))
            if len(data_buffer) >= max_buffer:
                print(f"Dumping buffer of size {len(data_buffer)} to file")
                write_to_file(output_path, data_buffer)
                data_buffer = []
    except KeyboardInterrupt:
        pass
    finally:
        write_to_file(output_path, data_buffer)
        device.close()

if __name__ == "__main__":
    main()
