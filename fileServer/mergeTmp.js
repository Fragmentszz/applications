const fs = require('fs-extra');
const path = require('path');


// 合并切片
const MergeTmp = async (chunkSize, fileHash, filePath) => {
    try {
      // target/chunkCache_fileHash值
      const chunkCache = getChunkDir(fileHash)
      // 读取 临时所有切片目录 chunkCache 下的所有文件和子目录，并返回这些文件和子目录的名称。
      const chunkPaths = await fse.readdir(chunkCache)
  
      // 根据切片下标进行排序
      // 否则直接读取目录的获得的顺序会错乱
      chunkPaths.sort((a, b) => a.split('-')[1] - b.split('-')[1])
  
      let promiseList = []
  
      for (let index = 0; index < chunkPaths.length; index++) {
        // target/chunkCache_hash值/文件切片位置
        let chunkPath = path.resolve(chunkCache, chunkPaths[index])
        // 根据 index * chunkSize 在指定位置创建可写流
        let writeStream = fse.createWriteStream(filePath, {
          start: index * chunkSize,
        })
        promiseList.push(pipeStream(chunkPath, writeStream))
      }
  
      // 使用 Promise.all 等待所有 Promise 完成
      // (相当于等待所有的切片已写入完成且删除了所有的切片文件)
      Promise.all(promiseList)
        .then(() => {
          // console.log('所有文件切片已成功处理并删除')
          // 在这里执行所有切片处理完成后的操作
          // 递归删除缓存切片目录及其内容 (注意，如果删除不存在的内容会报错)
          if (fse.pathExistsSync(chunkCache)) {
            fse.remove(chunkCache)
            // console.log(`chunkCache缓存目录删除成功`)
            // 合并成功，返回 Promise.resolve
            return Promise.resolve()
          } else {
            console.log(`${chunkCache} 不存在，不能删除`)
  
            return Promise.reject(`${chunkCache} 不存在，不能删除`)
          }
        })
        .catch((err) => {
          console.error('文件处理过程中发生错误：', err)
          // 在这里处理错误，可能需要清理资源等
          return Promise.reject(`'文件处理过程中发生错误：${err}`)
        })
    } catch (err) {
      console.log(err, '合并切片函数失败')
      return Promise.reject(`'合并切片函数失败：${err}`)
    }
  }
  

// 定义文件碎片目录和最终文件路径
const chunkDir = 'fileHash';
const filePath = 'a.raw';  // 这里可以替换为你想要的最终文件名
const chunkSize = 50 * 1024 * 1024;  // Chunk的大小，这里设置为1MB