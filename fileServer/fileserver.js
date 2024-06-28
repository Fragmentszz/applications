const express = require('express');
const path = require('path');
const fs = require('fs');
const app = express();
const {formatFileSize} = require('./util')

// 设置静态文件目录
app.use(express.static(path.join(__dirname, 'public')));
const filepath = "files"
// 文件上传处理
const multer = require('multer');
const upload = multer({ dest: 'uploads/' });
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));


// 上传文件的路由 
app.post('/upload', upload.single('file'), (req, res) => {
  if (req.file) {
    // 将文件从临时目录移动到静态文件目录
    const source = fs.createReadStream(req.file.path);
    const destination = fs.createWriteStream(path.join(filepath, req.file.originalname));
    source.pipe(destination);
    source.on('end', () => {
      fs.unlinkSync(req.file.path); // 删除临时文件
      res.send('File uploaded successfully');
    });
  } else {
    res.status(400).send('No file uploaded');
  }
});

// 下载文件的路由
app.get('/download/*', (req, res) => {
  const filename = req.path.substring(9);
  const filePath = path.join(filepath, filename);
  res.download(filePath, (err) => {
    if (err) {
      res.status(404).send('File not found');
    }
  });
});
const downDir = "http://downfile.fragments.work/";
// 列出文件的路由
app.get(/^\/(?!download\/).*/, (req, res) => {
  const directoryPath = path.join(__dirname, filepath, req.path);
  var stat = fs.lstatSync(directoryPath);
  if (stat.isFile()) {
    res.redirect(`/download/${req.path}`);
  }
  else {
    fs.readdir(directoryPath, (err, files) => {
      if (err) {
        res.status(500).send('Error reading directory');
        return;
      }
      // 获取文件信息
      const fileList = files.map(file => {
        const filePath = path.join(directoryPath, file);
        const stats = fs.statSync(filePath);
        return {
          name: file,
          path: path.join(req.path, file),
          size: formatFileSize(stats.size),
          type: path.extname(file).substring(1), // 获取文件类型
          isFile:stat.isFile()
        };
      });
      // 渲染EJS模板
      res.render('index', { files: fileList });
    });
  }
});


// 列出文件的路由
app.get('/file-list', (req, res) => {
  const directoryPath = path.join(__dirname, filepath);
  fs.readdir(directoryPath, (err, files) => {
    if (err) {
      res.status(500).send('Error reading directory');
      return;
    }
    res.json(files);
  });
});


// 启动服务器
const PORT = 6921;
app.listen(PORT, () => {
  console.log(`Server started on port ${PORT}`);
});
