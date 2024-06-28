const ejs = require('ejs');
const path = require('path');
const parseLL1AnalysisText = require('./predictionTable').parseLL1AnalysisText;

function processCFG(text) {
    // 解析文本
    const lines = text.trim().split('\n');
    let index = 0;

    // 解析基本信息
    const numNonTerminals = parseInt(lines[index++].trim(), 10);
    const nonTerminals = lines[index++].trim().split(' ');
    const numTerminals = parseInt(lines[index++].trim(), 10);
    const terminals = lines[index++].trim().split(' ');
    const startSymbol = lines[index++].trim();
    const numProductions = parseInt(lines[index++].trim(), 10);
    const productions = lines.slice(index, index + numProductions).map(production => {
        let [left, right] = production.trim().split('->');
        if (right === "") right = "epsilon";
        return { left, right: right.split('') };
    });
    index += numProductions;

    // 解析FIRST集合
    const firstSet = {};
    for (let i = 0; i < numNonTerminals; i++) {
        const firstLine = lines[index++].trim().split(':');
        const nonTerminal = firstLine[0].trim();
        const first = firstLine[1].replace('{', '').replace('}', '').split(',').map(s => s.trim());
        firstSet[nonTerminal] = new Set(first);
    }

    // 解析FOLLOW集合
    const followSet = {};
    for (let i = 0; i < numNonTerminals; i++) {
        const followLine = lines[index++].trim().split(':');
        const nonTerminal = followLine[0].trim();
        const follow = followLine[1].replace('{', '').replace('}', '').split(',').map(s => s.trim());
        followSet[nonTerminal] = new Set(follow);
    }

    // 解析SELECT集合
    const selectSet = {};
    for (let i = 0; i < numProductions; i++) {
        const selectLine = lines[index++].trim().split(':');
        const production = selectLine[0].trim();
        const select = selectLine[1].replace('{', '').replace('}', '').split(',').map(s => s.trim());
        selectSet[production] = new Set(select);
    }
    const isLL1 = parseInt(lines[index++].trim()[0]);
    // 创建文法对象
    const grammar = {
        nonTerminals,
        terminals,
        startSymbol,
        productions,
        firstSet,
        followSet,
        selectSet,
        isLL1
    };
    return grammar;
}




const express = require('express');
const multer = require('multer');
const upload = multer({ dest: 'uploads/' });
const { exec } = require('node:child_process');
const app = express();
let port = 3001;
if(process.argv[1].indexOf("pm2") < 0)
{
    port = 3002;
}

// 设置EJS为模板引擎
app.set('view engine', 'ejs');  
app.use('/', express.static(path.join(__dirname, 'public')));

const cppProgram = "./bin/LR"    /*别改*/
const cppProgram2 = "./bin/LL1"     /*可以改*/
// 定义一个路由
app.post('/read_CFG', upload.single('inputFile'), (req, res) => {
    const file = req.file;
    if (!file) {
        return res.status(400).send('没有上传文件。');
    }
    
    const outputImageFilePaths = exec(cppProgram + " " + `-filepath=${file.path} -mode=cacuset`, (err, stdout, stderr) => {
        if (err) {
            console.error(`执行错误: ${err}`);
            return res.status(500).send('处理文件失败。');
        }
        if (stderr) {
            console.error(`标准错误输出: ${stderr}`);
            return res.status(500).send('处理文件失败。');
        }
        console.log(stdout);
        // 假设这是你的grammar对象
        const grammar = processCFG(stdout);
        console.log(grammar);
        // 渲染ejs模板并传递grammar对象
        res.render('table', { grammar });
    });
});


function processDeduce(text) {
    // 将文本按行分割
    const lines = text.trim().split('\n');

    // 格式化每一行为对象
    const formatted = lines.map((line, index) => {
        // 分割每一行
        const parts = line.split(',').map(part => part.trim());
        // 提取 before 和 after
        const p = parts[0].replace('使用', '');
        const after = parts[1].replace('变为', '');
        // 返回格式化后的对象
        return {
            step: index + 1,
            p,
            after
        };
    });
    return formatted;
}


app.post('/deduce', upload.single('inputFile'), (req, res) => {
    // const derivationSteps = parseLL1AnalysisText(testtext);
    // res.render('deduce2',{...derivationSteps});
    // return;
    const file = req.file;
    if (!file) {
        return res.status(400).send('没有上传文件。');
    }
    const outputImageFilePaths = exec(cppProgram2 + " " + `-filepath=${file.path} -mode=derivation -s=${req.body.inputText}`, (err, stdout, stderr) => {
        if (err) {
            console.error(`执行错误: ${err}`);
            return res.status(500).send('处理文件失败。');
        }
        if (stderr) {
            console.error(`标准错误输出: ${stderr}`);
            return res.status(500).send('处理文件失败。');
        }
        // const derivationSteps = processDeduce(stdout);
        const tables = parseLL1AnalysisText(stdout);
        if (tables.successful) {
            const out2 = exec(cppProgram + " " + `-filepath=${file.path} -mode=derivation -s=${req.body.inputText}`, (err, stdout, stderr) => {
                if (err) {
                    console.error(`执行错误: ${err}`);
                    return res.status(500).send('处理文件失败。');
                }
                if (stderr) {
                    console.error(`标准错误输出: ${stderr}`);
                    return res.status(500).send('处理文件失败。');
                }
                const derivationSteps = processDeduce(stdout);
                res.render('deduce2',{...tables,derivationSteps});
            });
        } else {
            res.render('deduce2', { ...tables});
        }


    });
});
const fs = require('fs');
// 清理上传目录的定时任务
const cleanupUploads = () => {
    const directory = 'uploads/';
    const oneDay = 24 * 60 * 60 * 1000; // 一天的毫秒数
    const maxAge = oneDay*2; // 文件最大存储时间为7天
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
    const downym = "downfile.fragments.work/";
    const example = "examples/cp/LL1_example1.txt"
    const redirectUrl = `http://${downym}${example}`;
    res.redirect(302, redirectUrl);
});

// 设置定时任务，每天执行一次清理操作
setInterval(cleanupUploads, 24*60*60*24); // 每24小时执行一次


const LRapp = require('./LRserver.js');

app.use(LRapp);


// 启动服务器
app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});
