function formatFileSize(bytes) {
    if (bytes === 0) return '0 B';
    const units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    const exponent = Math.floor(Math.log(bytes) / Math.log(1024));
    const unit = units[exponent];
    const size = (bytes / Math.pow(1024, exponent)).toFixed(2);
    return `${size}${unit}`;
}
const path = require('path');
const fs = require('fs');
const downdir = "http://downfile.fragments.work";
function getPrelook(extension, urlpath) {
    // var stat = fs.lstatSync(extension);

    if (extension == "") {
        return "/icons/default.png";
    }
    if (extension == ".jpg") {
        return downdir + urlpath;
    }
    switch (extension) {
        case '.ppt':
        case '.pptx':
            return '/icons/ppt.png'; // 假设PPT图标路径是/icons/ppt.png
        case '.doc':
        case '.docx':
            return '/icons/docx.png'; // 假设DOCX图标路径是/icons/docx.png
        case '.xls':
        case '.xlsx':
            return '/icons/excel.png'; // 假设XLSX图标路径是/icons/excel.png
        case '.jpg':
        case '.png':

            return downdir + urlpath;
        case '.mp4':
            return "/icons/video.png"
        default:
            return '/icons/default.png'; // 默认图标路径
    }
}

module.exports = {
    formatFileSize: formatFileSize,
    getPrelook: getPrelook
}