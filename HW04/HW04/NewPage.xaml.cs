using Newtonsoft.Json;
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

namespace HW04 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class NewPage : Page {
        public NewPage() {
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
                composite["Field1"] = TodoTitle.Text;
                composite["Field2"] = TodoDetails.Text;
                composite["Field3"] = JsonConvert.SerializeObject(TodoDate.Date);
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

                    TodoTitle.Text = (string)composite["Field1"];
                    TodoDetails.Text = (string)composite["Field2"];
                    TodoDate.Date = JsonConvert.DeserializeObject<DateTimeOffset>((string)composite["Field3"]);
                    
                    // We're done with it, so remove it
                    ApplicationData.Current.LocalSettings.Values.Remove("TheWorkInProgress");
                }
            }
        }

        /* 创建按钮-点击事件 */
        private void TodoCreate_Click(object sender, RoutedEventArgs e) {
            String str = string.Empty;
            /* 检查文本是否为空 */
            if (TodoTitle.Text == string.Empty || TodoDetails.Text == string.Empty) {
                str += "Empty Information!\n";
            }
            /* 检查日期是否合法 */
            if (TodoDate.Date < DateTime.Now.Date) {
                str += "Error Date!";
            }
            /* 对话框提醒 */
            if (str != string.Empty) {
                var i = new MessageDialog(str).ShowAsync();
            }
        }

        /* 取消按钮-点击事件 */
        private void TodoCancel_Click(object sender, RoutedEventArgs e) {
            TodoTitle.Text = string.Empty; /* 清理文本 */
            TodoDetails.Text = string.Empty; /* 清理文本 */
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
                    TodoImage.Source = bitmapImage;
                }
            }
        }
    }
}
