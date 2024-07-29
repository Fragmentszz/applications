
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


// const form = document.getElementById('uploadForm');
// const progressBar = document.getElementById('progressBar');
// const ahas = document.getElementById('hasUpload');
// const atot = document.getElementById('totalSize');


// let xhr = null;
// let isPaused = false;
// let isCancelled = false;
// let path = 'asdlfal'
// let formData = null;
// form.addEventListener('submit', function (event) {
//     event.preventDefault();
//     formData = new FormData(form);
//     xhr = new XMLHttpRequest();
//     xhr.open('POST', path, true);
//     // 上传进度事件
//     xhr.upload.onprogress = function (e) {
//         if (e.lengthComputable) {
//             atot.innerHTML = getFileSize(e.total);
//             ahas.innerHTML = getFileSize(e.loaded);
//             progressBar.value = (e.loaded / e.total) * 100;
//         }
//     };

//     // 请求完成
//     xhr.onload = function () {
//         if (xhr.status === 200) {
//             alert('File uploaded successfully');
//             location.reload(true); // 传入true作为参数，将强制从服务器加载页面
//         } else {
//             alert('An error occurred!');
//         }
//     };

//     // 发送请求
//     xhr.send(formData);

// });



// document.getElementById('pauseBtn').addEventListener('click', pauseUpload);
// document.getElementById('cancelBtn').addEventListener('click', cancelUpload);

// function pauseUpload() {
//     if (xhr) {
//         if (xhr.readyState !== XMLHttpRequest.DONE) {
//             if (!isPaused) {
//                 xhr.abort(); // 取消当前请求
//                 isPaused = true;
//             } else {
//                 xhr.open('POST', path, true);
//                 xhr.send(formData);
//                 isPaused = false;
//             }
//         }
//     }
// }

// function cancelUpload() {
//     if (xhr) {
//         xhr.abort(); // 取消当前请求
//         isCancelled = true;
//         document.getElementById('progressBar').value = 0;
//         atot.innerHTML = '--';
//         ahas.innerHTML = '--';
//     }
// }
