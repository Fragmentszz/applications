const morgan = require('morgan');
const fs = require('fs');
const path = require('path');

// 创建一个日志文件
const logDirectory = path.join(__dirname, 'log');
if (!fs.existsSync(logDirectory)) {
  fs.mkdirSync(logDirectory);
}
const accessLogStream = fs.createWriteStream(path.join(logDirectory, 'access.log'), { flags: 'a' });

// 使用morgan将所有console.log重定向到access.log文件
morgan.token('date', function () { return new Date().toISOString(); });
morgan.format('combined', ':date[iso] :remote-addr :method :url :status :res[content-length] - :response-time ms');

app.use(morgan('combined', { stream: accessLogStream }));

// 或者，您可以使用morgan.token来定制日志输出
app.use(morgan(':date[iso] :remote-addr :method :url :status :res[content-length] - :response-time ms', { stream: accessLogStream }));

// 然后您可以在代码中使用console.log来输出信息，这些信息会被重定向到access.log文件
console.log('This message will be logged to access.log');
