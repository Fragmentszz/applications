const express = require('express')
const app = express()
const path = require('path')
const fse = require('fs-extra')
const multiparty = require('multiparty')
const {delDir,modetoBin, movetoBin} = require('./util.js');
const {Password} = require('./config.json');
app.use(express.json());
app.use((req, res, next) => {
  // 请求头允许跨域
  res.setHeader('Access-Control-Allow-Origin', '*')
  res.setHeader('Access-Control-Allow-Headers', '*')
  next()
})

app.options('*', (req, res) => {
  res.sendStatus(200)
})



// 大文件存储目录
const BIN_DIR = path.resolve(__dirname, 'rubbishbin');
const FILE_DIR = path.resolve(__dirname, 'files')


app.post('/delete', async (req,res) => {
    // console.log(req);
    const { password, filepath } = req.body; // 解构赋值提取password和filepath
    
    if (password === Password) {
        try {
            const realpath = path.join(FILE_DIR, filepath);
          // 调用 delDir 函数删除文件或目录
          await movetoBin(realpath,filepath,BIN_DIR)
          // 如果没有错误，返回成功消息
          res.send(
            {
                status:'0',
                'describe':'removed into rubbish bin!'
            }
          );
        } catch (err) {
          // 处理其他可能发生的错误
          console.error(err);
          res.status(200).send(
            {
                status:'1',
                'describe':err.toString()
            }
          );
        }
      } else {
        // 密码不正确时，返回错误
        res.status(200).send({
            status:'1',
            'describe':'password error!'
        });
      }


});

module.exports = app;