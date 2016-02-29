using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW01 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        /* 枚举类型，作为事件参数 */
        enum AnimalType { PIG, DOG, CAT };

        /* 委托 */
        private delegate void AnimalSaying(object sender, myEventArgs e);
        /* 事件 */
        private event AnimalSaying Say;

        public MainPage() {
            this.InitializeComponent();
        }

        /* 接口 */
        interface Animal {
            void saying(object sender, myEventArgs e); /* 方法 */
        }

        class Pig : Animal {
            TextBlock word;

            public Pig(TextBlock tb) {
                this.word = tb;
            }

            public void saying(object sender, myEventArgs e) {
                if (e.flag) {
                    this.word.Text += "pig:"; /* 点击确认按钮时添加说话动物类型，下同 */
                }
                this.word.Text += "I am a pig.\n";
            }
        }

        class Dog : Animal {
            TextBlock word;

            public Dog(TextBlock tb) {
                this.word = tb;
            }

            public void saying(object sender, myEventArgs e) {
                if (e.flag) {
                    this.word.Text += "dog:";
                }
                this.word.Text += "I am a dog.\n";
            }
        }

        class Cat : Animal {
            TextBlock word;

            public Cat(TextBlock tb) {
                this.word = tb;
            }

            public void saying(object sender, myEventArgs e) {
                if (e.flag) {
                    this.word.Text += "cat:";
                }
                this.word.Text += "I am a cat.\n";
            }
        }

        /* 用于获取说话的动物类型的函数 */
        private AnimalType GetAnimalType(string input) {
            if (input != "") {
                if (input == "pig") {
                    return AnimalType.PIG; /* 返回PIG */
                } else if (input == "dog") {
                    return AnimalType.DOG; /* 返回DOG */
                } else if (input == "cat") {
                    return AnimalType.CAT; /* 返回CAT */
                }
            }
            /* 生成一个小于枚举类型长度的随机整数，强制转换为枚举类型并返回 */
            Random rd = new Random();
            return (AnimalType)rd.Next(Enum.GetNames(typeof(AnimalType)).Length);
        }

        private Pig p;
        private Dog d;
        private Cat c;

        /* 发言按钮点击 */
        private void say_Click(object sender, RoutedEventArgs e) {
            AnimalType judge = GetAnimalType("");
            switch (judge) {
                case AnimalType.PIG:
                    if (p == null) {
                        p = new Pig(words); /* 初始化，下同 */
                    }
                    Say = new AnimalSaying(p.saying); /* 注册事件，下同 */
                    break;
                case AnimalType.DOG:
                    if (d == null) {
                        d = new Dog(words);
                    }
                    Say = new AnimalSaying(d.saying);
                    break;
                case AnimalType.CAT:
                    if (c == null) {
                        c = new Cat(words);
                    }
                    Say = new AnimalSaying(c.saying);
                    break;
                default:
                    break;
            }
            Say(this, new myEventArgs(false)); /* 执行事件 */
        }

        /* 确认按钮点击 */
        private void ack_Click(object sender, RoutedEventArgs e) {
            AnimalType judge = GetAnimalType(member.Text); /* 获取动物类型 */
            switch (judge) {
                case AnimalType.PIG:
                    if (p == null) {
                        p = new Pig(words); /* 初始化，下同 */
                    }
                    Say = new AnimalSaying(p.saying); /* 注册事件，下同 */
                    break;
                case AnimalType.DOG:
                    if (d == null) {
                        d = new Dog(words);
                    }
                    Say = new AnimalSaying(d.saying);
                    break;
                case AnimalType.CAT:
                    if (c == null) {
                        c = new Cat(words);
                    }
                    Say = new AnimalSaying(c.saying);
                    break;
                default:
                    break;
            }
            words.Text = ""; /* 清空TextBlock */
            member.Text = ""; /* 清空TextBox */
            Say(this, new myEventArgs(true)); /* 执行事件 */
        }

        //自定义一个Eventargs传递事件参数
        class myEventArgs : EventArgs {
            public bool flag = false;
            public myEventArgs(bool f) {
                this.flag = f;
            }
        }
    }
}
