
let modalHeight = document.getElementById('modal').offsetHeight;
document.getElementById('modal').style.marginTop = -(modalHeight / 2) + "px";

function fnClose() {
    document.getElementById('modal').setAttribute("class", "hidden");
    document.getElementById('body').setAttribute("class", "body-class");
};




function changeSizeFormat(fileSizeBytes) {
    const fileSizeKB = fileSizeBytes / 1024; // 转换为KB
    const fileSizeMB = fileSizeKB / 1024; // 转换为MB
    const fileSizeGB = fileSizeMB / 1024; // 转换为GB
    // 根据文件大小选择合适的单位
    if (fileSizeGB >= 1) {
        return { num: fileSizeGB, dw: "GB" };
    } else if (fileSizeMB >= 1) {
        return { num: fileSizeMB, dw: "MB" };
    } else {
        return { num: fileSizeKB, dw: "KB" };
    }
}
function getFileSize(fileSizeByte) {
    const fileSize = changeSizeFormat(fileSizeByte);
    return `${fileSize.num.toFixed(2)} ${fileSize.dw} `;
}
function displayFileSize() {
    const fileInput = document.getElementById('fileInput');
    const fileInfo = document.getElementById('fileInfo');
    // 确保选择了文件
    if (fileInput.files.length > 0) {
        const file = fileInput.files[0];
        const fileSizeBytes = file.size;
        fileInfo.textContent = `File size: ` + getFileSize(fileSizeBytes);
    } else {
        fileInfo.textContent = 'No file selected.';
    }
}

let ison = false;
// 当用户点击上传按钮时，打开iframe
function showUploadIframe() {
    console.log("?");
    var iframe = document.getElementById('modal');
    iframe.style.display = 'block'; // 显示iframe
    iframe.src = '/uploadFile.html'; // 设置iframe的src属性
    ison = true;
}
// 隐藏iframe的函数
function hideUploadIframe() {
    var iframe = document.getElementById('modal');
    iframe.style.display = 'none'; // 隐藏iframe
    iframe.src = ''; // 清空iframe的src属性，避免保留旧内容
    ison = false;
}

hideUploadIframe();
// 添加事件监听器以侦听整个页面的点击事件
document.addEventListener('click', function (event) {
    // 检查点击的元素是否是你想要排除的元素或其子元素
    var isClickInsideElement = event.target.closest('#modal') !== null;
    var isClickInsideButton = event.target.closest('#showButton') !== null;
    // 如果点击的不是iframe或其子元素，则隐藏iframe
    if (!isClickInsideButton && !isClickInsideElement) {
        hideUploadIframe();
    }
});

document.addEventListener('DOMContentLoaded', function () {
    // 删除按钮点击事件
    document.querySelectorAll('.delete-btn').forEach(button => {
        button.addEventListener('click', function () {
            const filePath = this.getAttribute('data-file-path'); // 假设按钮有一个data-file-path属性
            const p = document.getElementById('ensure-text');
            p.innerText = "注意！您将会删除" + filePath + "";
            const dialog = document.getElementById('confirm-delete');
            dialog.returnValue = filePath; // 设置返回值
            dialog.showModal();
        });
    });

    // 确认删除按钮点击事件
    document.getElementById('confirm-btn').addEventListener('click', function () {
        const filePath = document.getElementById('confirm-delete').returnValue;
        console.log(filePath);
        deleteFile(filePath); // 调用删除文件的函数
        document.getElementById('confirm-delete').close();
    });

    // 取消按钮点击事件
    document.getElementById('cancel-btn').addEventListener('click', function () {
        document.getElementById('confirm-delete').close();
    });
});

function deleteFile(filePath){
    const password = document.getElementById('delete-pswd').value;

    // 创建要发送的数据对象
    const data = {
      password: password,
      filepath: filePath
    };

    // 发送POST请求到'/delete'
    axios.post('/delete', data)
      .then(function (response) {
        const res = response.data;
        desciption = res['describe'];
        if(res.status === '0'){
            alert('已移入回收站~最多存放7天！');
        }else{
            alert(desciption);
        }

        location.reload();
      })
      .catch(function (error) {
        console.log(error);
        alert("发生了通信故障=.=");

        location.reload();
      });
}