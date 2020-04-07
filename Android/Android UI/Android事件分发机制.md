## Android事件分发机制

- 事件流向

  - Activity->ViewGroup->View

- Down事件

  - 对于 dispatchTouchEvent，onTouchEvent，return true是终结事件传递。return false 是回溯到父View的onTouchEvent方法。
  - ViewGroup 想把自己分发给自己的onTouchEvent，需要拦截器onInterceptTouchEvent方法return true 把事件拦截下来。
  - ViewGroup 的拦截器onInterceptTouchEvent 默认是不拦截的，所以return super.onInterceptTouchEvent()=return false，事件给到子view
  - View 没有拦截器，为了让View可以把事件分发给自己的onTouchEvent，View的dispatchTouchEvent默认实现（super）就是把事件分发给自己的onTouchEvent。

- dispatchTouchEvent 是做事件分发

  - 自己消费，终结传递。------->return true ；

  -  给自己的onTouchEvent处理-------> 调用super.dispatchTouchEvent()系统默认会去调用 onInterceptTouchEvent，在onInterceptTouchEvent return true就会去把事件分给自己的onTouchEvent处理。

  - 传给子View------>调用super.dispatchTouchEvent()默认实现会去调用 onInterceptTouchEvent 在onInterceptTouchEvent return false，就会把事件传给子类。不传给子View，事件终止往下传递，事件开始回溯，从父View的onTouchEvent开始事件从下到上回归执行每个控件的onTouchEvent------->return false；

    **注：** 由于View没有子View所以不需要onInterceptTouchEvent 来控件是否把事件传递给子View还是拦截，所以View的事件分发调用super.dispatchTouchEvent()的时候默认把事件传给自己的onTouchEvent处理（相当于拦截），对比ViewGroup的dispatchTouchEvent 事件分发，View的事件分发没有上面提到的4个目标的第3点。

- onTouchEvent方法是做事件处理的
  - 自己消费掉，事件终结，不再传给谁----->return true;
  - 继续从下往上传，不消费事件，让父View也能收到到这个事件----->return false;View的默认实现是不消费的。所以super==false。
- ViewGroup的onInterceptTouchEvent方法对于事件有两种情况：
  - 拦截下来，给自己的onTouchEvent处理--->return true;
  - 不拦截，把事件往下传给子View---->return false,ViewGroup默认是不拦截的，所以super==false；



#### 关于ACTION_MOVE 和 ACTION_UP####

- ACTION_DOWN事件在哪个控件消费了（return true），  那么ACTION_MOVE和ACTION_UP就会从上往下（通过dispatchTouchEvent）做事件分发往下传，就只会传到这个控件，不会继续往下传，如果ACTION_DOWN事件是在dispatchTouchEvent消费，那么事件到此为止停止传递，如果ACTION_DOWN事件是在onTouchEvent消费的，那么会把ACTION_MOVE或ACTION_UP事件传给该控件的onTouchEvent处理并结束传递。