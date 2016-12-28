import("Log.js");
import("Timer.js");


function onTime()
{
	print("On Time");
}
function applicationDidStart()
{
    
    addLocalLogger();

    print("Hello World! \n");

    var timer = new Timer( 1000 , 1000 , "onTime");
    timer.start();
//    quit();
}
