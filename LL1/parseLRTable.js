// 解析文本并构建LR分析表
function parseLRTable(text) {
    const lineso = text.trim().split('\n');
    const lines = lineso.map(item => {
        return item.trim();
    }
    );

    let stateTotal = parseInt(lines[0], 10);
    let stateIndex = 1;
    let items = [];

    // 解析状态和项目
    while (stateIndex < lines.length && lines[stateIndex].startsWith('I')) {

        let itemLine = lines[stateIndex].split(':');
        let state = parseInt(itemLine[0].substring(1), 10);
        let statenum = parseInt(itemLine[1], 10);
        items[state] = [];
        stateIndex++;
        let cnt = 0;
        // 解析该状态的所有项目
        while (cnt < statenum) {
            items[state].push(lines[stateIndex]);
            stateIndex++;
            cnt++;
        }
    }

    // 解析表格尺寸
    let tableSizeLine = lines[stateIndex].split(' ');
    let actionRows = parseInt(tableSizeLine[0], 10);
    let actionCols = parseInt(tableSizeLine[1], 10);
    let gotoCols = parseInt(tableSizeLine[2], 10);
    stateIndex++;

    // 解析终结符和非终结符
    let symbols = lines[stateIndex].split(' ');
    stateIndex++;

    // 解析Action和GOTO表
    let Table = Array.from({ length: actionRows }, () => Array(actionCols + gotoCols).fill(null));
    while (stateIndex < lines.length) {
        let tableEntry = lines[stateIndex].split(' ');
        let state = parseInt(tableEntry[0], 10);
        let symbolIndex = parseInt(tableEntry[1], 10);
        let value = tableEntry[2].replace('{', '').replace('}', '').split(',');
        let values = [];
        value.forEach(element => {
            values.push(parseInt(element));
        });
        Table[state][symbolIndex] = values;
        stateIndex++;
    }

    return {
        stateTotal,
        items,
        Table,
        symbols,
        actionCols,
        gotoCols
    };
}


// LR分析过程的文本表示


// 解析LR分析过程的函数
function parseLRAnalysis(text) {
    // 将文本按行分割
    const lines = text.trim().split('\n');

    // 存储解析结果的数组
    const results = [];

    // 遍历每一行
    lines.forEach(line => {
        // 分割每一行的内容
        const [stateStack, symbolStack, input, action, goto] = line.split(' ');

        // 解析Action和GOTO
        let actionValue, gotoValue;
        actionValue = parseInt(action,10);
        if (isNaN(actionValue)) {
            actionValue = action;
        }

        // 将解析结果存储到数组中
        results.push({
            stateStack: stateStack != undefined ? stateStack : "",
            symbolStack: symbolStack != undefined ? symbolStack : "",
            input: input != undefined ? input : "",
            action: actionValue,
            goto: goto != undefined ? goto : ""
        });
    });

    return { steps: results };
}
// 解析LR分析过程的函数
function parseSemantics(text) {
    // 将文本按行分割
    const lines = text.trim().split('\n');

    // 存储解析结果的数组
    const results = [];

    // 遍历每一行
    lines.forEach(line => {
        // 分割每一行的内容
        const [stateStack, symbolStack,semanticsStack,input, action, goto,semanticsAction] = line.split(' ');

        // 解析Action和GOTO
        let actionValue, gotoValue;
        actionValue = parseInt(action,10);
        if (isNaN(actionValue)) {
            actionValue = action;
        }

        // 将解析结果存储到数组中
        results.push({
            stateStack: stateStack != undefined ? stateStack : "",
            symbolStack: symbolStack != undefined ? symbolStack : "",
            semanticsStack: semanticsStack,
            input: input != undefined ? input : "",
            action: actionValue,
            goto: goto != undefined ? goto : "",
            semanticsAction:semanticsAction != undefined? semanticsAction:""
        });
    });

    return { steps: results };
}



module.exports = {
    parseLRTable: parseLRTable,
    parseLRAnalysis: parseLRAnalysis,
    parseSemantics:parseSemantics
}