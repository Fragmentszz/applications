  
  document.getElementById('imageForm').addEventListener('submit', function(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    fetch('/generate-images', {
        method: 'POST',
        body: formData
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json();
    })
    .then(js => {
        let imgs = js["imgs"];
        console.log(js);
        console.log(imgs);
        let imagesContainer = null;
        if(document.getElementById("imgContainer") !== null){
            imagesContainer = document.getElementById("imgContainer");
            while(imagesContainer.firstChild){
                imagesContainer.removeChild(imagesContainer.firstChild);
            }
        }else{
            imagesContainer = document.createElement('div');
            imagesContainer.id = "imgContainer";
        }
        const imgnames = ['NFA.png', 'DFA.png', 'DFA_min.png'];
        // 清除之前的图片
        for (let i = 0; i < 3; i++) {
            let imageUrl = "./imgs/" + imgs[i];
            const img = document.createElement('img');
            img.src = imageUrl;
            img.alt = imgnames[i];
            const imageName = document.createElement('h2');
            imageName.textContent = imgnames[i];
            const imageDiv = document.createElement('div');
            imageDiv.classList.add('center');
            imageDiv.appendChild(imageName);
            imageDiv.appendChild(img);
            imagesContainer.appendChild(imageDiv);
        }
        document.body.appendChild(imagesContainer);
    })
    .catch(error => {
        console.error('There was a problem with the fetch operation:', error);
    });
});


function testText() {
    const inputText = document.getElementById('inputText').value;
    const formData = new FormData(document.getElementById('imageForm'));
    formData.append('text', inputText);
    fetch('/test-text', {
        method: 'POST',
        body: formData
    })
    .then(response => response.json())
    .then(result => {
        const testResult = document.getElementById('testResult');
        const message = `<span class="red-text">${result.accepted ? 'Accepted: ' : 'Rejected: '}</span> 字符串"${inputText}"${result.accepted ?'能被该自动机接受' : '不能被该自动机接受'}`;
        testResult.innerHTML = message;
    })
    .catch(error => {
        console.error('Error:', error);
    });
}