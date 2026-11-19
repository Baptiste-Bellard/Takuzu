Module.onRuntimeInitialized = () => { 
    g = Module._new_default();
    canvas = document.getElementById("Mycanvas");

    solve = document.getElementById("Solve");
    solve.addEventListener('click',function() {sol(g);});   

    restart = document.getElementById("Restart");
    restart.addEventListener('click',function() {res(g);});    

    undo = document.getElementById("Undo");
    undo.addEventListener('click',function() {und(g);});   

    redo = document.getElementById("Redo");
    redo.addEventListener('click',function() {red(g);});   

    ctx = canvas.getContext('2d');
    nb_rows = Module._nb_rows(g);
    nb_cols = Module._nb_cols(g);
    cell_size = Math.min(window.innerWidth/nb_cols, window.innerHeight/nb_rows)/2;
    canvas.width = cell_size * nb_cols;
    canvas.height = cell_size * nb_rows;

    canvas.addEventListener("click", function(event) {
        var rect = canvas.getBoundingClientRect();
        var x = event.clientX - rect.left;
        var y = event.clientY - rect.top;

        var i = Math.floor(y / cell_size);
        var j = Math.floor(x / cell_size);
        
        var number = Module._get_number(g, i, j);
        var immutable = Module._is_immutable(g, i, j);

        if(!immutable){
            if(number == -1){
                Module._play_move(g, i, j, 1);
            }
            if(number == 0){
                Module._play_move(g, i, j, 2);
            }
            if(number == 1){
                Module._play_move(g, i, j, 0);
            }    
        }
        printGame(g);
    });
    
    start();
    
}

function printGame(g) {
    var cell_size_width = canvas.width/nb_cols;
    var cell_size_height =canvas.height/nb_rows;
    for (var i = 0; i < nb_rows; i++) {
        for (var j = 0; j < nb_cols; j++) {
            var number = Module._get_number(g, i, j);
            var immutable = Module._is_immutable(g,i,j);
            var error = Module._has_error(g, i, j);
            if(Module._is_empty(g,i,j)){
                ctx.fillStyle = "grey";
                ctx.fillRect(j*cell_size_height,i*cell_size_width,cell_size_height,cell_size_width);
            }
            if (immutable && number == 0){
                ctx.fillStyle = "#BADA55";
                ctx.fillRect(j*cell_size_height,i*cell_size_width,cell_size_height,cell_size_width);
            } 
            else if (immutable && number == 1){
                ctx.fillStyle = "blue"
                ctx.fillRect(j*cell_size_height,i*cell_size_width,cell_size_height,cell_size_width);
            }   
            else if (number == 0){
                ctx.fillStyle = "#BADA55"
                ctx.fillRect(j*cell_size_height,i*cell_size_width,cell_size_height,cell_size_width);
            }
            else if (number == 1){
                ctx.fillStyle = "blue"
                ctx.fillRect(j*cell_size_height,i*cell_size_width,cell_size_height,cell_size_width);
            }
            if(error){
                DrawCross(i*cell_size_height,j*cell_size_width,cell_size_height,cell_size_width);
            }          
        }
    }
    if(Module._is_over(g)){
        console.log("Congratulation !");
        DrawVictory();
    }     
}


function DrawVictory() {
    ctx.fillStyle = "black";
    ctx.font = "bold 50px Arial";
    ctx.textAlign = "center";
    ctx.fillText("Congratulations!", canvas.width/2, canvas.height/2);

    ctx.fillStyle = "black";
    ctx.font = "bold 30px Arial";
    ctx.fillText("You won the game!", canvas.width/2, canvas.height/2 + 40);
}

function DrawCross(i, j, h, w) {
    // Dessine la première ligne de la croix 
    ctx.beginPath();
    ctx.moveTo(j, i);
    ctx.lineTo(j + w, i + h);
    ctx.lineWidth = 2;
    ctx.strokeStyle = "red";
    ctx.stroke();
    // Dessine la deuxième ligne de la croix 
    ctx.beginPath();
    ctx.moveTo(j + w, i);
    ctx.lineTo(j, i + h);
    ctx.lineWidth = 2;
    ctx.strokeStyle = "red";
    ctx.stroke();
}

function sol(g){
    Module._solve(g);
    printGame(g);
}

function res(g){
    Module._restart(g);
    printGame(g);
}

function und(g){
    Module._undo(g);
    printGame(g);
}

function red(g){
    Module._redo(g);
    printGame(g);
}

function start() {
    printGame(g);  
}