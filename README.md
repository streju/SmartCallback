# SmartCallback
### Description

Callback that notify (assert) when callable object which it refers was not called and it lifetime expires (last reference to object is removed). 
SmartCallback is a class that wrapps std::function object in the similar way as shared_ptr keeps pointed object. Its definition holds pointer to structure that have information about nr of objects that currently refer to the same std::function, whole structure is shared between instances of SmartCallback. During removal of last object check if refered std::function was called is performed.

### Usage
It can be helpful during code refactoring or any changes in projects where occurs a lot of nested callbacks (callback hell). In such projects there is a risk that some action can be lost on lower level function call and lead to application stuck. Using SmartCallback any lost action is easy to find during implementation but it also forces that tests must cover the entire scenario.
