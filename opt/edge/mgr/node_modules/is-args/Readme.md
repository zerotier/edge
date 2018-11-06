# is-args

Quickly check if a variable is Arguments.

```javascript

var isArgs = require('is-args');

isArgs((function () { return arguments; })(1, 2, 3));
// true;

isArgs([1, 2, 3]);
// false;

```

## Installation

    $ npm install is-args

## Limitations

This is not the standard `Object.prototype.toString.call(val) === '[object Arguments]'` method.
This method is [much faster](http://jsperf.com/is-args-vs-object-prototype-tostring-call) than the `toString` method, 
however, there is a chance that you may get false positives on objects that resemble arguments:

```javascript

var x = [1,2,3];
x.callee = function () {};

isArguments(x);
// true

```

Keep this in mind as you work with this module.