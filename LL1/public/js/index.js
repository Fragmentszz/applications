function openNewWindow(htmlContent) {
    const newWindow = window.open('', '_blank'); // 打开新窗口
    newWindow.document.write(htmlContent); // 将HTML内容写入新窗口的文档中
    newWindow.document.close(); // 关闭文档写入，使新窗口的文档生效
}

document.getElementById('imageForm').addEventListener('submit', function (event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    fetch('/read_CFG', {
        method: 'POST',
        body: formData
    }).then(response => response.text())
        .then(data => {
            // 这里处理返回的HTML页面
            console.log(data);
            openNewWindow(data);
        })
        .catch(error => {
            // 处理错误
            console.error('Error:', error);
        });
});

document.getElementById('deduce').addEventListener('click', function (event) {
    event.preventDefault();
    const formData = new FormData(document.getElementById('imageForm'));
    fetch('/deduce', {
        method: 'POST',
        body: formData
    }).then(response => response.text())
        .then(data => {
            // 这里处理返回的HTML页面
            console.log(data);
            openNewWindow(data);
        })
        .catch(error => {
            // 处理错误
            console.error('Error:', error);
        });
});

var kmode = "LR0";
var radios = document.querySelectorAll('input[name="parserMode"]');
function onChange(event) {
  var value = event.target.value;
  kmode = value;
}
radios.forEach(function(radio) {
  radio.addEventListener('change', onChange);
});
// 事件监听器函数
function handleButtonClick(event) {
    // 获取按钮的模式
    const mode = event.target.getAttribute('mode');
    const action = event.target.getAttribute('class');
    document.getElementById('mode').value = kmode;
    document.getElementById('action').value = action;
    event.preventDefault();
    const formData = new FormData(document.getElementById('imageForm'));
    fetch('/LR', {
        method: 'POST',
        body: formData
    }).then(response => response.text())
        .then(data => {
            // 这里处理返回的HTML页面
            console.log(data);
            openNewWindow(data);
        })
        .catch(error => {
            // 处理错误
            console.error('Error:', error);
        });
    console.log(action,mode);
}

function handleButtonClick2(event) {
    // 获取按钮的模式
    const action = event.target.getAttribute('class');
    document.getElementById('mode').value = kmode;
    document.getElementById('action').value = action;
    document.getElementById('semantic').value = "true";
    event.preventDefault();
    const formData = new FormData(document.getElementById('imageForm'));
    fetch('/LR', {
        method: 'POST',
        body: formData
    }).then(response => response.text())
        .then(data => {
            // 这里处理返回的HTML页面
            console.log(data);
            openNewWindow(data);
        })
        .catch(error => {
            // 处理错误
            console.error('Error:', error);
        });
    console.log(action,kmode);
    document.getElementById('semantic').value = "";
}

const buttons = document.querySelectorAll('.table, .deduce');
// 为每个按钮添加事件监听器
buttons.forEach(button => {
  button.addEventListener('click', handleButtonClick);
});

document.getElementById('semanticbutton').addEventListener('click', handleButtonClick2);