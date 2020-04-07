Activity四种启动模式

- ### Standard

  - 每次都创建一个新的实例

- ### SingleTask

  - 没有就创建一个
  - 如果任务栈中有就弹出顶部的activity，onNewIntent()方法会被调用

- ### SingleTop 

  - 如果任务栈顶部有，则复用，onNewIntent()方法会被调用
  - 如果没有就创建

- ### SingleInstance 

  - 如果任务栈中没有实例，就创建新任务栈，新创建activity入栈
  - 如果有，则复用，onNewIntent()方法会被调用
  - SingleInstance activity中创建**`Standard`** 模式activity时 启动其他 Activity 时会默认带有 `FLAG_ACTIVITY_NEW_TASK` 属性。所以 Activity E 启动 Activity F 后，最后会存在三个任务栈，Activity F 会单独存在于一个任务栈中

