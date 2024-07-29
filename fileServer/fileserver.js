const express = require('express');
const path = require('path');
const fs = require('fs');
const app = express();
const { formatFileSize,getPrelook } = require('./util')
const uploadServer = require('./uploadserver.js');
// 设置静态文件目录
app.use(express.static(path.join(__dirname, 'public')));
const filepath = "files"
// 文件上传处理
const multer = require('multer');
const upload = multer({ dest: 'uploads/' });
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

const pswd_set = "wxhna3590";
// // 上传文件的路由 
// app.post('/upload/*', upload.single('file'), (req, res) => {
//   const filedir = path.join(filepath,req.path.substring(8));
//   const pswd = req.body.pswd;
//   if(pswd != pswd_set){
//     res.status(400).send('Password Denied!');
//     return;
//   }
//   if (req.file) {
//     // 将文件从临时目录移动到静态文件目录
//     const source = fs.createReadStream(req.file.path);
//     const destination = fs.createWriteStream(path.join(filedir, req.file.originalname));
//     source.pipe(destination);
//     source.on('end', () => {
//       fs.unlinkSync(req.file.path); // 删除临时文件
//       res.send('File uploaded successfully');
//     });
//   } else {
//     res.status(400).send('No file uploaded');
//   }
// });
app.get('*/uploadfile.html', (req, res) => {
  const reqdir = req.path.substring(0,req.path.length - 16) + '/';
  const filedir = path.join(filepath,reqdir);
  return res.render('uploadfile',{path:reqdir});
});

app.use(uploadServer);
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
  const extension = path.extname(directoryPath).toLowerCase();
  var stat = fs.lstatSync(directoryPath);
  if (stat.isFile()) {
    const filePath = directoryPath;
    const stats = fs.statSync(filePath);
    const ejsfile = {
      name: path.basename(directoryPath),
      path: req.path,
      time: stats.atime,
      size: formatFileSize(stats.size),
      type: path.extname(directoryPath).substring(1), // 获取文件类型
      downurl: `/download${req.path}`,
      openurl: downDir + req.path,
      prelook: getPrelook(extension,req.path),
      extension:extension
    }
    res.render('File', ejsfile);
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
        let t = "forder";
        if (stat.isFile()) {
          t = path.extname(file).substring(1); // 获取文件类型
        }
        // 获取文件的最后修改时间（毫秒数）
        const mtimeMs = stats.mtimeMs + 8*60*60*1000;
        // 将毫秒数转换为可读的日期和时间
        const mtimeDate = new Date(mtimeMs);
        return {
          name: file,
          path: path.join(req.path, file),
          time: mtimeDate.toISOString().replace('T', ' ').slice(0, 19),
          size: formatFileSize(stats.size),
          type: t,
          isFile: stats.isFile()
        };
      });
      // 渲染EJS模板
      res.render('index', { files: fileList,nowpath:req.path.substring(1),lastpath:path.dirname(req.path) });
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
