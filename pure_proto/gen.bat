"..\tools\protoc.exe" --cpp_out .\pb ^
    -I ..\protocols\extensions ^
    -I ..\protocols ^
    -I ..\xres-code-generator\pb_extension ^
    ..\protocols\extensions\google\protobuf\descriptor.proto ^
    ..\protocols\extensions\v3\xresloader.proto ^
    ..\protocols\extensions\v3\xresloader_ue.proto ^
    ..\xres-code-generator\pb_extension\xrescode_extensions_v3.proto ^
    ..\protocols\pb_header_v3.proto