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



// 如果你想要取消定时器，可以这样做：
// clearInterval(timer);



const fse = require('fs-extra');
const moment = require('moment'); // 用于时间处理

async function isDirectoryEmpty(dirPath) {
    try {
        // 使用fs.promises.readdir获取文件夹中的所有文件和目录的名称
        const files = await fse.readdir(dirPath);
        // 如果返回的数组长度为0，则文件夹为空
        return files.length === 0;
    } catch (err) {
        // 如果发生错误，则文件夹不为空
        return false;
    }
}

async function delDir(p) {
    // 读取文件夹中所有文件及文件夹
    const list = await fse.readdir(p);

    for (const v of list) {
        const url = p + '/' + v;
        const stats = await fse.stat(url);

        // 判断是文件还是文件夹
        if (stats.isFile()) {
            // 当前为文件，则删除文件
            await fs.unlink(url);
        } else {
            // 当前为文件夹，则递归调用自身
            await delDir(url);
        }
    }

    // 删除空文件夹
    await fse.rmdir(p);
}


async function deleteFilesAndFolder(uploadsDir, maxAge) {
    try {
        // 获取文件夹中的所有文件和目录
        const files = await fse.readdir(uploadsDir);

        // 遍历文件和目录
        for (const file of files) {
            const filePath = path.join(uploadsDir, file);
            const stats = await fse.lstat(filePath);

            // 如果是文件，并且其修改时间超过最大年龄
            if (stats.isFile() && moment(stats.mtime).isBefore(moment().subtract(maxAge, 'seconds'))) {
                // 删除文件
                await fse.unlink(filePath);
                console.log(`已删除旧文件: ${filePath}`);
            }

            // 如果是目录，并且其修改时间超过最大年龄
            if (stats.isDirectory() && moment(stats.mtime).isBefore(moment().subtract(maxAge, 'seconds'))) {
                delDir(filePath);
            }
        }
    } catch (err) {
        console.error('删除文件和文件夹时发生错误:', err);
    }
}

function clearnUp(dictionary, maxAge, timeInterval) {
    let ft = function () {
        deleteFilesAndFolder(dictionary, maxAge);
    };
    const timer = setInterval(ft, timeInterval);
}

module.exports = {
    formatFileSize: formatFileSize,
    getPrelook: getPrelook,
    clearUploadsDirectory: clearnUp,
    delDir:delDir
}