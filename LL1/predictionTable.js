function parseLL1AnalysisText(text) {
    let lines = text.split('\n');
    for (let i = 0; i < lines.length; i++) {
        lines[i] = lines[i].trim();
    }
    const [numNonTerminals, numTerminals] = lines[0].split(' ').map(Number);
    const nonTerminals = lines[1].split(' ');
    const terminals = lines[2].split(' ');
    const startSymbol = nonTerminals[0];
    let Terminals2 = [];
    terminals.forEach(element => {
        if (element != "epsilon")
            Terminals2.push(element);
    });
    // 构建预测分析表
    const table = {};
    for (let i = 0; i < numNonTerminals; i++) {
        for (let j = 0; j < numTerminals + 1; j++) {
            const production = lines[i * (numTerminals + 1) + j + 3].trim();
            const nonTerminal = nonTerminals[i];
            const terminal = terminals[j] || 'epsilon';
            if (terminal == "epsilon") {
                continue;
            }
            if (!table[nonTerminal]) {
                table[nonTerminal] = {};
            }
            table[nonTerminal][terminal] = production;
        }
    }
    let process = [];
    const numStackRow = parseInt(lines[3 + numNonTerminals * (numTerminals + 1)]);
    const base = 3 + numNonTerminals * (numTerminals + 1)+1;
    let successful = 1;
    for (let i = 0; i < numStackRow; i++) { 
        const t = lines[base + i].split(' ');
        const stack = t[0],nows = t[1];
        let action = "";
        for(let j = 2;j<t.length;j++){
            action += t[j];
        }
        process.push({
            stack,nows,action
        });
        if(action.startsWith('Syntax')){
            successful = 0;
        }
    }
    if(!successful){
        return {
            nonTerminals: nonTerminals, Terminals2, table,process,successful,error:process[process.length-1].action.replace('SyntaxError:',"")
        };
        
    }
    return {
        nonTerminals: nonTerminals, Terminals2, table,process,successful
    };

}
module.exports = {
    parseLL1AnalysisText
};
// 给定的文本

