const express = require('express');
const multer = require('multer');
const fs = require('fs');
const { JSDOM } = require('jsdom');
const { window } = new JSDOM();
const { document } = (new JSDOM('')).window;
global.document = document;
const {exec} = require('node:child_process');
const { send } = require('process');
const app = express();
const upload = multer({ dest: 'uploads/' });

const crypto = require('crypto');
const path = require('path');
// 假设这是您的C++程序的路径
const cppProgram = './DFA ';



// 生成图片名
function generateImageName(inputText) {
    // 使用 MD5 哈希函数对输入文本进行哈希处理
    const hash = crypto.createHash('md5').update(inputText).digest('hex');
    // 生成一个唯一的名字，例如 'image_' + hash
    const imageName = 'image_' + hash;
    return imageName;
  }


// // 处理文件上传的POST请求
// app.post('/generate-images', upload.single('inputFile'), (req, res) => {
//     const currentTime = new Date();
//     const currentTimeString = currentTime.toLocaleString();
//     res.send({
//         "imgs":[generateImageName(currentTimeString + "NFA.png"),generateImageName(currentTimeString + "DFA.png"),generateImageName(currentTimeString + "DFA_min.png"),]
//     });
// });

// 处理文件上传的POST请求
app.post('/generate-images', upload.single('inputFile'), (req, res) => {
    const file = req.file;
    if (!file) {
        return res.status(400).send('没有上传文件。');
    }
    fs.readFile(file.path, 'utf8', (err, data) => {
        if (err) {
            return res.status(500).send('读取文件失败。');
        }
        // 调用C++程序来处理TXT文件并生成图像
        const outputImageFilePaths = exec(cppProgram + " " + file.path, (err, stdout, stderr) => {
            if (err) {
                console.error(`执行错误: ${err}`);
                return res.status(500).send('处理文件失败。');
            }
            if (stderr) {
                console.error(`标准错误输出: ${stderr}`);
                return res.status(500).send('处理文件失败。');
            }
            const currentTime = new Date();
            const currentTimeString = currentTime.toLocaleString();
            // 获取输出图像文件的路径
            const outputImageFilePaths = ["NFA.png","DFA.png","DFA_min.png"];
            const sendNames = [generateImageName(currentTimeString + "NFA.png"),generateImageName(currentTimeString + "DFA.png"),generateImageName(currentTimeString + "DFA_min.png")];
            const targetDir = './imgs/';
            // 发送图像文件给客户端
            for(let i=0;i<3;i++) {
                let fileName = sendNames[i];
                let oriname = outputImageFilePaths[i];
                const targetFilePath = targetDir + fileName;
                // 复制文件
                fs.copyFile(oriname, targetFilePath, (err) => {
                    if (err) {
                        console.error(`复制文件 "${fileName}" 失败:${err}`);
                        return;
                    }
                    console.log(`文件 "${fileName}" 复制成功。`);
                });
            }
            res.send({
                "imgs":sendNames
            });
        });
    });
});

function runDFACheck(inputText, filepath, res) {
    const command = `./DFA_check ${filepath} ${inputText}`;
    exec(command, (err, stdout, stderr) => {
        if (err) {
            console.error('Error executing DFA_check:', err);
            return res.status(500).json({ error: 'Error executing DFA_check.' });
        }
        const resultCode = Number(stdout.trim());
        const accepted = resultCode === 1;
        res.json({ accepted });
    });
}

// POST endpoint for testing text
app.post('/test-text', upload.single('inputFile'), (req, res) => {
    let inputText = req.body.text;
    const file = req.file;
    fs.readFile(file.path, 'utf8', (err, data) => {
        if (!inputText) {
            if (!req.file) {
                return res.status(400).json({ error: 'No text or file provided.' });
            }
            fs.readFile(req.file.path, 'utf8', (err, data) => {
                if (err) {
                    return res.status(500).json({ error: 'Failed to read file.' });
                }
                inputText = data;
                runDFACheck(inputText, file.path,res);
            });
        } else {
            runDFACheck(inputText, file.path,res);
        }
    });
    
});



const PORT = 3000;

app.use('/',express.static(path.join(__dirname,'html')));
app.use('/imgs/',express.static(path.join(__dirname,'imgs')));
app.listen(PORT, () => {
    console.log(`服务器运行在 http://localhost:${PORT}`);
});

// 清理上传目录的定时任务
const cleanupUploads = () => {
    const directory = 'uploads/';
    const oneDay = 24 * 60 * 60 * 1000; // 一天的毫秒数
    const maxAge = 7 * oneDay; // 文件最大存储时间为7天

    fs.readdir(directory, (err, files) => {
        if (err) {
            console.error('Error reading directory:', err);
            return;
        }

        const currentTime = Date.now();

        files.forEach(file => {
            const filePath = path.join(directory, file);
            fs.stat(filePath, (err, stats) => {
                if (err) {
                    console.error('Error getting file stats:', err);
                    return;
                }

                const fileAge = currentTime - stats.mtime.getTime();
                if (fileAge > maxAge) {
                    fs.unlink(filePath, err => {
                        if (err) {
                            console.error('Error deleting file:', err);
                            return;
                        }
                    });
                }
            });
        });
    });
};

app.get('/download_example', (req, res) => {
    const filePath = path.join(__dirname, 'html','example.txt');
    const fileName = 'example.txt';

    res.download(filePath, fileName, (err) => {
        if (err) {
            console.error('下载文件时出错:', err);
            res.status(500).send('下载文件时出错。');
        } else {
            // console.log('文件下载成功。');
        }
    });
});

// 设置定时任务，每天执行一次清理操作
setInterval(cleanupUploads, 24 * 60 * 60 * 1000); // 每24小时执行一次
