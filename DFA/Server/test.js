const { exec } = require('node:child_process');


exec('echo "The \\$HOME variable is $HOME"');

exec('"./DFA" minimize_example_1.txt',(err,stdout,stderr)=>{
	if(err){
	    console.log(err);
	    return;
	}
	console.log(stdout);
	console.log(stderr);
    }
);
// Double quotes are used so thati the space in the path is not interpreted as
// a delimiter of multiple arguments.

exec('echo "The \\$HOME variable is $HOME"');
// The $HOME variable is escaped in the first instance, but not in the second.
