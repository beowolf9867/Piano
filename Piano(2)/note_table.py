import math

#constants

F_CPU = 12000000
PRESCALER = 8
NUM_KEYS = 128
START_NOTE_INDEX = 12 #middle c

def calculate_ocr(frequency):
    if frequency <= 0:
        return 0
    ocr_val = (F_CPU / (2*PRESCALER*frequency)) - 1
    return int(round(ocr_val))

def get_frequency(n):
    return 440 * (math.pow(2, (n - 49) / 12))

print(f"const uint16_t note_table[{NUM_KEYS}] = {{")

for i in range(NUM_KEYS):
    freq = get_frequency(START_NOTE_INDEX + i) 
    ocr = calculate_ocr(freq)

    comma = "," if i < NUM_KEYS - 1 else ""

    print(f"    {ocr}{comma}")

print("};")
