const express = require('express');
const multer = require('multer');
const upload = multer({ dest: 'uploads/' });
const { exec } = require('node:child_process');
const {parseLRTable,parseLRAnalysis,parseSemantics} = require('./parseLRTable');
const path = require('path');
// 假设文本存储在变量text中
const LR_path = './bin/LR'
const app = express();

app.set('view engine', 'ejs');  

app.use('/', express.static(path.join(__dirname, 'public')));

app.post('/LR', upload.single('inputFile'), (req, res) => {
    const file = req.file;
    
    if (!file) {
        return res.status(400).send('没有上传文件。');
    }
  if(req.body.semantic=="true"){
    const outputImageFilePaths = exec(LR_path + " " + `-filepath=${file.path} -mode=${req.body.mode} -s=${req.body.inputText} -semantic=true`, (err, stdout, stderr) => {
      if (err) {
        console.error(`执行错误: ${err}`);
        return res.status(500).send('执行错误: ${err}');
      }
      if (stderr) {
        console.error(`标准错误输出: ${stderr}`);
        return res.status(500).send('标准错误输出: ${stderr}');
      }
      res.render('semantics',parseSemantics(stdout));
    });
  }
  const outputImageFilePaths = exec(LR_path + " " + `-filepath=${file.path} -mode=${req.body.action + '_' + req.body.mode} -s=${req.body.inputText}`, (err, stdout, stderr) => {
    if (err) {
      console.error(`执行错误: ${err}`);
      return res.status(500).send('执行错误: ${err}');
    }
    if (stderr) {
      console.error(`标准错误输出: ${stderr}`);
      return res.status(500).send('标准错误输出: ${stderr}');
    }
    if (req.body.action == "table") {
      res.render('lr_table',parseLRTable(stdout));
    } else if (req.body.action == "deduce") {
      res.render('lr_deduce',parseLRAnalysis(stdout));
    }
  });
});

module.exports = app;