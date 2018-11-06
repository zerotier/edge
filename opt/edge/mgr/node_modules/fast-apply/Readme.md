# fast-apply

Apply an array of arguments... faster.

```javascript

var fastApply = require('fast-apply')
  , scope = {}
;

function run() {}

fastApply(run, scope, [ 'a', 'b', 'c' ]);

```

## Installation

    $ npm install fast-apply

## Dependencies

none

## Speed

It uses a simple switch statement to `call` the function rather than `apply` for up to 5 arguments.

While more than 5 arguments would run faster using `apply`, anything less will be 
[up to twice as fast](http://jsperf.com/many-arguments-apply-vs-switch/2)

If the scope is `null`, fastApply will run up to 3 times faster than `apply`

## Limitations

  - It is only optimized for 5 arguments or less. If you think the function will frequently be taking
  more than 5 arguments, I suggest that you instead use `apply`