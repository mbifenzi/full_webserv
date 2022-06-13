
#include "../Utils.h"


std::string     getMimeType(const std::string & file)
{
    std::map<std::string , std::string> map;
    std::map<std::string , std::string>::iterator it;

    map["html"] = "text/html";
    map["htm"] = "text/html";
    map["css"] = "text/css";
    map["js"] = "application/javascript";
    map["jpg"] = "image/jpeg";
    map["jpeg"] = "image/jpeg";
    map["png"] = "image/png";
    map["gif"] = "image/gif";
    map["ico"] = "image/x-icon";
    map["txt"] = "text/plain";
    map["pdf"] = "application/pdf";
    map["mp3"] = "audio/mpeg";
    map["mp4"] = "video/mp4";
    map["wav"] = "audio/wav";
    map["ogg"] = "audio/ogg";
    map["mpg"] = "video/mpeg";
    map["mpeg"] = "video/mpeg";
    map["mov"] = "video/quicktime";
    map["swf"] = "application/x-shockwave-flash";
    map["zip"] = "application/zip";
    map["tar"] = "application/x-tar";
    map["gz"] = "application/x-gzip";
    map["rar"] = "application/x-rar-compressed";
    map["bz2"] = "application/x-bzip2";
    map["7z"] = "application/x-7z-compressed";
    map["doc"] = "application/msword";
    map["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    map["xls"] = "application/vnd.ms-excel";
    map["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    map["ppt"] = "application/vnd.ms-powerpoint";
    map["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    map["odt"] = "application/vnd.oasis.opendocument.text";
    map["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    map["odp"] = "application/vnd.oasis.opendocument.presentation";
    map["odg"] = "application/vnd.oasis.opendocument.graphics";
    map["odc"] = "application/vnd.oasis.opendocument.chart";
    map["odf"] = "application/vnd.oasis.opendocument.formula";
    map["odb"] = "application/vnd.oasis.opendocument.database";
    map["odi"] = "application/vnd.oasis.opendocument.image";
    map["oxt"] = "application/vnd.openofficeorg.extension";
    map["rtf"] = "application/rtf";
    map["xml"] = "text/xml";
    map["svg"] = "image/svg+xml";
    map["svgz"] = "image/svg+xml";
    map["ttf"] = "application/x-font-ttf";
    map["otf"] = "application/x-font-otf";
    map["woff"] = "application/font-woff";
    map["woff2"] = "application/font-woff2";
    map["eot"] = "application/vnd.ms-fontobject";
    map["sfnt"] = "application/font-sfnt";
    map["wasm"] = "application/wasm";
    map["json"] = "application/json";
    map["xml"] = "text/xml";
    map["mp4"] = "video/mp4";
    map["webm"] = "video/webm";
    map["ogv"] = "video/ogg";
    map["ogx"] = "application/ogg";
    map["ogg"] = "audio/ogg";
    map["oga"] = "audio/ogg";
    map["ogm"] = "video/ogg";
    map["ogx"] = "application/ogg";
    map["opus"] = "audio/ogg";
    map["flac"] = "audio/flac";
    map["flv"] = "video/x-flv";
    map["m4v"] = "video/x-m4v";
    map["m4a"] = "audio/x-m4a";
    map["m4b"] = "audio/x-m4b";
    map["m4p"] = "audio/x-m4p";
    map["m4r"] = "audio/x-m4r";
    map["m4v"] = "video/x-m4v";
    map["3gp"] = "video/3gpp";
    map["3g2"] = "video/3gpp2";
    map["3gpp"] = "video/3gpp";
    map["3gpp2"] = "video/3gpp2";
    map["3gp2"] = "video/3gpp2";

    if ((it = map.find(file.substr(file.find_last_of(".") + 1))) != map.end())
        return it->second;
    return "text/plain";
}