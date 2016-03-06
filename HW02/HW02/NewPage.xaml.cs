using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// “空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=234238 上提供

namespace HW02 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class NewPage : Page {
        public NewPage() {
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
            var i = new MessageDialog("Welcome!").ShowAsync();
        }

        /* 创建按钮-点击事件 */
        private void TodoCreate_Click(object sender, RoutedEventArgs e) {
            String str = "";
            /* 检查文本是否为空 */
            if (TodoTitle.Text == "" || TodoDetails.Text == "") {
                str += "Empty Information!\n";
            }
            /* 检查日期是否合法 */
            if (TodoDate.Date < DateTime.Now.Date) {
                str += "Error Date!";
            }
            /* 对话框提醒 */
            if (str != "") {
                var i = new MessageDialog(str).ShowAsync();
            }
        }

        /* 取消按钮-点击事件 */
        private void TodoCancel_Click(object sender, RoutedEventArgs e) {
            TodoTitle.Text = ""; /* 清理文本 */
            TodoDetails.Text = ""; /* 清理文本 */
            TodoDate.Date = DateTime.Now.Date; /* 重设日期 */
        }

        /* 选择图片按钮-点击事件 */
        private async void SelectPictureButton_Click(object sender, RoutedEventArgs e) {
            FileOpenPicker fp = new FileOpenPicker();
            /* 选择浏览模式-缩略图 */
            fp.ViewMode = PickerViewMode.Thumbnail;
            /* 选择默认路径-图片库 */
            fp.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            /* 选择器-添加文件格式 */
            fp.FileTypeFilter.Add(".jpg");
            fp.FileTypeFilter.Add(".jpeg");
            fp.FileTypeFilter.Add(".png");
            fp.FileTypeFilter.Add(".bmp");
            /* 选择图片 */
            StorageFile file = await fp.PickSingleFileAsync();
            /* 确认选择成功 */
            if (file != null) {
                using (IRandomAccessStream fileStream = await file.OpenAsync(FileAccessMode.Read)) {
                    /* 加载图片 */
                    BitmapImage bitmapImage = new BitmapImage();
                    await bitmapImage.SetSourceAsync(fileStream);
                    Image.Source = bitmapImage;
                }
            }
        }
    }
}
