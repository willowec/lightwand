# Image format

Images used to generate C arrays to represent cahracters on the light wand must:

1. Have a filename that is also a valid C variable name
2. Be of width and height CHAR_WIDTH and CHAR_HEIGHT as defined in [generate_alphabet.py](../generate_alphabet.py)
3. Consist of only black ('on') and white ('off') pixels