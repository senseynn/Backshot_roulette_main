# Assets

This game has **no external image or audio assets**.

All graphics are generated procedurally using SFML drawing primitives:
- Shotgun, table, felt, lamp, shell indicators — all drawn with sf::RectangleShape / sf::CircleShape
- Health bars, UI buttons, item slots — SFML shapes

All sounds are generated procedurally using `sf::SoundBuffer::loadFromSamples()`:
- Gunshot (noise burst with exponential decay)
- Blank shot (filtered noise)
- Shell eject (frequency sweep)
- Item use (ding tones)
- Heal, handcuffs, phone, beer — synthesized waveforms

No PNG, JPEG, OGG, WAV, or TTF files are bundled.
Font is searched from system fonts at runtime (DejaVu, Liberation, Ubuntu, Arial, Helvetica...).
