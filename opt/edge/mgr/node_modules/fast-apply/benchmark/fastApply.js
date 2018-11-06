var Benchmark = require('../node_modules/benchmark')
  , fastApply = require('..')

  , options = { onCycle: cycle, onComplete: complete }
  , fastest = []

  , context = {
      a: 1,
      b: 2,
      c: 3,
  }

  , oneArg = [ 1 ]
  , fiveArgs = oneArg.concat([ 2, 3, 4, 5 ])
  , sixArgs = fiveArgs.concat([ 6 ])
;

function run() {}

function cycle(e) {
    console.log(this.name, String(e.target));
}

function complete() {
    fastest.push(this.name + ': ' + this.filter('fastest').pluck('name')[0]);
}

Benchmark.Suite('0 args, no context', options)
  .add('apply', function () {
      run.apply(null);
  })
  .add('fastApply', function () {
      fastApply(run, null);
  })
  .run()
;

Benchmark.Suite('0 args, context', options)
  .add('apply', function () {
      run.apply(context);
  })
  .add('fastApply', function () {
      fastApply(run, context);
  })
  .run()
;

Benchmark.Suite('1 arg, no context', options)
  .add('apply', function () {
      run.apply(null, oneArg);
  })
  .add('fastApply', function () {
      fastApply(run, null, oneArg);
  })
  .run()
;

Benchmark.Suite('1 arg, context', options)
  .add('apply', function () {
      run.apply(context, oneArg);
  })
  .add('fastApply', function () {
      fastApply(run, context, oneArg);
  })
  .run()
;

Benchmark.Suite('5 args, no context', options)
  .add('apply', function () {
      run.apply(null, fiveArgs);
  })
  .add('fastApply', function () {
      fastApply(run, null, fiveArgs);
  })
  .run()
;

Benchmark.Suite('5 args, context', options)
  .add('apply', function () {
      run.apply(context, fiveArgs);
  })
  .add('fastApply', function () {
      fastApply(run, context, fiveArgs);
  })
  .run()
;

Benchmark.Suite('6 args, no context', options)
  .add('apply', function () {
      run.apply(null, sixArgs);
  })
  .add('fastApply', function () {
      fastApply(run, null, sixArgs);
  })
  .run()
;

Benchmark.Suite('6 args, context', options)
  .add('apply', function () {
      run.apply(context, sixArgs);
  })
  .add('fastApply', function () {
      fastApply(run, context, sixArgs);
  })
  .run()
;

console.log('Fastest:');
fastest.forEach(function (text) {
    console.log('   ' + text);
});