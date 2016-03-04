using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW02 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            Frame rootFrame = Window.Current.Content as Frame;

            if (rootFrame.CanGoBack) {
                // Show UI in title bar if opted-in and in-app backstack is not empty.
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Visible;
            } else {
                // Remove the UI from the title bar if in-app back stack is empty.
                SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                    AppViewBackButtonVisibility.Collapsed;
            }
        }

        /* 添加按钮-新建页面 */
        private void AppBarButton_Click(object sender, RoutedEventArgs e) {
            /* 传参为空 */
            Frame.Navigate(typeof(NewPage));
        }

        /* CheckBox被选择->设置可见 */
        private void checkBox1_Checked(object sender, RoutedEventArgs e) {
            this.Line1.Visibility = Visibility.Visible;
        }

        /* CheckBox被取消->设置隐藏 */
        private void checkBox1_Unchecked(object sender, RoutedEventArgs e) {
            this.Line1.Visibility = Visibility.Collapsed;
        }

        /* CheckBox被选择->设置可见 */
        private void checkBox2_Checked(object sender, RoutedEventArgs e) {
            this.Line2.Visibility = Visibility.Visible;
        }

        /* CheckBox被取消->设置隐藏 */
        private void checkBox2_Unchecked(object sender, RoutedEventArgs e) {
            this.Line2.Visibility = Visibility.Collapsed;
        }
    }
}
