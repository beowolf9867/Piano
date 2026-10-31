import math

F_CPU = 20000000
PRESCALER = 8

def get_ocr_value(frequency):
    if frequency == 0: return 0
    # ctc toggle mode
    return int((F_CPU / (2 * PRESCALER * frequency)) - 1)

with open("note_table2.h", "w") as f:
    f.write("// Frequency table for Timer1 (Prescaler 8)\n")
    f.write("const uint16_t note_table[] = {\n")
    
    for midi_note in range(128):
        freq = 440.0 * math.pow(2, (midi_note - 69) / 12.0)
        ocr_val = get_ocr_value(freq)
        
        f.write(f"    {ocr_val}, // MIDI {midi_note} ({freq:.2f} Hz)\n")
        
    f.write("};\n")

print("note_table2.h generated successfully!")
