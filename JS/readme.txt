/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <manuel.deneu@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
/*
*/

Natives functions
--------------------------
function exec(jsCode); // execute the given code
function eval(jsCode); // execute the given string (an expression) and return the result
function trace();
function Object.dump();
function Object.clone();
function Math.rand();
function Math.randInt(min, max);
function charToInt(ch); //  convert a character to an int - get its value
function String.indexOf(search); // find the position of a string in a string, -1 if not
function String.substring(lo,hi);
function String.charAt(pos);
function String.charCodeAt(pos);
function String.fromCharCode(char);
function String.split(separator);
function Integer.parseInt(str); string to int
function Integer.valueOf(str); // value of a single character
function JSON.stringify(obj, replacer); // convert to JSON. replacer is ignored at the moment

function Array.contains(obj);
function Array.remove(obj);
function Array.join(separator);

Maths
--------------------------
function Math.abs(a);
function Math.round(a);
function Math.min(a,b);
function Math.max(a,b);
function Math.range(x,a,b);
function Math.sign(a);
function Math.PI();
function Math.toDegrees(a);
function Math.toRadians(a);
function Math.sin(a);
function Math.asin(a);
function Math.cos(a);
function Math.acos(a);
function Math.tan(a);
function Math.atan(a);
function Math.sinh(a);
function Math.asinh(a);
function Math.cosh(a);
function Math.acosh(a);
function Math.tanh(a);
function Math.atanh(a);
function Math.E();
function Math.log(a);
function Math.log10(a);
function Math.exp(a);
function Math.pow(a,b);
function Math.sqr(a);
function Math.sqrt(a);


