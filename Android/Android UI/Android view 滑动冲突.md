- **滑动冲突场景**：内外两层View都可以滑动时候，就会产生滑动冲突

  ![img](https://upload-images.jianshu.io/upload_images/2839355-91541788260cb2de.png?imageMogr2/auto-orient/strip|imageView2/2/w/466/format/webp)

  - **外层与内层滑动方向不一致**，外层ViewGroup是可以横向滑动的，内层View是可以竖向滑动的（类似ViewPager，每个页面里面是ListView）
    - **根据当前滑动方向，水平还是垂直来判断这个事件到底该交给谁来处理**
      - 定义两个点，根据水平和竖直方向滑动距离来判断
      - 水平或竖直方向夹角，或者滑动速度
  - **外层与内层滑动方向一致**，外层ViewGroup是可以竖向滑动的，内层View同样也是竖向滑动的（类似ScrollView包裹ListView）



- 滑动冲突解决套路
  - 外部拦截法
    - 父View根据需要对事件进行拦截。逻辑处理放在父View的onInterceptTouchEvent方法中。
      - 根据业务逻辑需要，在ACTION_MOVE方法中进行判断，如果需要父View处理则返回true，否则返回false，事件分发给子View去处理。
      - ACTION_DOWN 一定返回false，不要拦截它，否则根据View事件分发机制，后续ACTION_MOVE 与 ACTION_UP事件都将默认交给父View去处理！
      - 原则上ACTION_UP也需要返回false，如果返回true，并且滑动事件交给子View处理，那么子View将接收不到ACTION_UP事件，子View的onClick事件也无法触发。而父View不一样，如果父View在ACTION_MOVE中开始拦截事件，那么后续ACTION_UP也将默认交给父View处理！
  - 内部拦截法
    - 即父View不拦截任何事件，所有事件都传递给子View，子View根据需要决定是自己消费事件还是给父View处理。这需要子View使用requestDisallowInterceptTouchEvent方法才能正常工作。
      - 父View需要重写onInterceptTouchEvent方法：
    - 内部拦截法要求父View不能拦截ACTION_DOWN事件，由于ACTION_DOWN不受FLAG_DISALLOW_INTERCEPT标志位控制，一旦父容器拦截ACTION_DOWN那么所有的事件都不会传递给子View。
    - 滑动策略的逻辑放在子View的dispatchTouchEvent方法的ACTION_MOVE中，如果父容器需要获取点击事件则调用 parent.requestDisallowInterceptTouchEvent(false)方法，让父容器去拦截事件。

