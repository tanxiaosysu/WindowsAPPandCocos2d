using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW04 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();
            ViewModel = new ViewModels.TheViewModel();
            DataContext = ViewModel;
            ViewModel.LoadData();
        }

        ViewModels.TheViewModel ViewModel {
            get; set;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e) {
            bool suspending = ((App)App.Current).IsSuspending;
            if (suspending) {
                // Save volatile state in case we get terminated later on, then
                // we can restore as if we'd never been gone :)
                var composite = new ApplicationDataCompositeValue();
                composite["Field1"] = (bool)checkBox1.IsChecked ? "True" : string.Empty;
                composite["Field2"] = (bool)checkBox2.IsChecked ? "True" : string.Empty;
                ApplicationData.Current.LocalSettings.Values["TheWorkInProgress"] = composite;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            if (e.NavigationMode == NavigationMode.New) {
                // If this is a new navigation, this is a fresh launch so we can
                // discard any saved state
                ApplicationData.Current.LocalSettings.Values.Remove("TheWorkInProgress");
            } else {
                // Try to restore state if any, in case we were terminated
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("TheWorkInProgress")) {
                    var composite = ApplicationData.Current.LocalSettings.Values["TheWorkInProgress"] as ApplicationDataCompositeValue;

                    if ((string)composite["Field1"] != string.Empty) {
                        this.checkBox1.IsChecked = true;
                        this.Line1.Visibility = Visibility.Visible;
                    } else {
                        this.checkBox1.IsChecked = false;
                        this.Line1.Visibility = Visibility.Collapsed;
                    }

                    if ((string)composite["Field2"] != string.Empty) {
                        this.checkBox2.IsChecked = true;
                        this.Line2.Visibility = Visibility.Visible;
                    } else {
                        this.checkBox2.IsChecked = false;
                        this.Line2.Visibility = Visibility.Collapsed;
                    }

                    // We're done with it, so remove it
                    ApplicationData.Current.LocalSettings.Values.Remove("TheWorkInProgress");
                }
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
