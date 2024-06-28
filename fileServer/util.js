function formatFileSize(bytes) {
    if (bytes === 0) return '0 B';
    const units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    const exponent = Math.floor(Math.log(bytes) / Math.log(1024));
    const unit = units[exponent];
    const size = (bytes / Math.pow(1024, exponent)).toFixed(2);
    return `${size}${unit}`;
}

module.exports = {
    formatFileSize:formatFileSize
}