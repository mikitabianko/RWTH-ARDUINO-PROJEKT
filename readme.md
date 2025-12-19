System::input.joystickButton.pressed<br>
System::input.joystickButton.released<br>
System::input.joystickButton.held

System::input.analogButtons[0...4].pressed<br>
System::input.analogButtons[0...4].released<br>
System::input.analogButtons[0...4].held

System::input.joystick.moved<br>
System::input.joystick.up.entered<br>
System::input.joystick.up.exited<br>
System::input.joystick.up.held<br>
System::input.joystick.down.entered<br>
System::input.joystick.down.exited<br>
System::input.joystick.down.held<br>
System::input.joystick.left.entered<br>
System::input.joystick.left.exited<br>
System::input.joystick.left.held<br>
System::input.joystick.right.entered<br>
System::input.joystick.right.exited<br>
System::input.joystick.right.held

System::input.joystick.lastUpdated

```cpp
if (System::input.joystick.lastUpdated == System::Direction::Up) {
    // Do something
}
```