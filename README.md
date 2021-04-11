# FilePatcher

Simple program to patch files using a config.

# How to use   

  * Edit the config.json and add new data entries
  * Drag the file to be patched over the .exe

# Config file documentation

  - **backup** - When true the original file will be copied and renamed to `file_name.extension.bak` and the original file will be the one modified.
  - **data** - Is an array that contains entries with a start_offset and values fields.
  - **start_offset** - This field will dictated at what offset the data will start to be writen.
  - **values** - Array of bytes (in hex) that will be writen in the file.
  
 *The initial config file already contains 2 entries as an example. Feel free to change it.*
