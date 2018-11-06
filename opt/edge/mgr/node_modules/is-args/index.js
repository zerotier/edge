module.exports = function (val) {
    return !!val
      && typeof val.length === 'number'
      && typeof val.callee === 'function'
    ;
};