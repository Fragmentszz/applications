(function(){"use strict";self.importScripts("spark-md5.min.js");function l(e,s){return new Promise((t,n)=>{let r=[],a=0;for(;a<e.size;)r.push({chunkFile:e.slice(a,a+s)}),a+=s;t(r)})}async function f(e){const s=new self.SparkMD5.ArrayBuffer;let t=0;async function n(r){return r>=e.length?s.end():new Promise((a,i)=>{const o=new FileReader;o.readAsArrayBuffer(e[r].chunkFile),o.onload=c=>{s.append(c.target.result),t+=100/e.length,self.postMessage({percentage:t}),a(n(r+1))},o.onerror=c=>{i(c)}})}try{const r=await n(0);self.postMessage({percentage:100,fileHash:r,fileChunkList:e}),self.close()}catch(r){self.postMessage({name:"error",data:r}),self.close()}}self.addEventListener("message",async e=>{try{const{file:s,chunkSize:t}=e.data,n=await l(s,t);await f(n)}catch(s){console.error("worker监听发生错误:",s)}},!1),self.addEventListener("error",function(e){console.log("Worker触发主线程的error事件：",e),self.close()})})();
