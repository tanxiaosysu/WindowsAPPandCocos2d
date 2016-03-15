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

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW03 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();
            this.ViewModel = new ViewModels.TodoListItemViewModel();
            TodoItemEditorInit();
        }

        private ViewModels.TodoListItemViewModel ViewModel;

        /* TodoItem初始化函数 */
        private void TodoItemEditorInit() {
            /* 清空按钮 */
            EditorEdit.Click -= TodoCreateClick;
            EditorEdit.Click -= TodoUpdateClick;
            /* 设置按钮 */
            if (ViewModel.SelectedItem == null) {
                EditorEdit.Content = "Create";
                EditorEdit.Click += TodoCreateClick;
                /* 设置值 */
                EditorTitle.Text = "";
                EditorDetails.Text = "";
                EditorDate.Date = DateTime.Today;
            } else {
                EditorEdit.Content = "Update";
                EditorEdit.Click += TodoUpdateClick;
                /* 设置值 */
                EditorTitle.Text = ViewModel.SelectedItem.title;
                EditorDetails.Text = ViewModel.SelectedItem.description;
                EditorDate.Date = ViewModel.SelectedItem.date;
            }
        }

        /* 内容检查函数-内容完整返回true */
        private bool ItemContentJudge() {
            String str = "";
            /* 检查文本是否为空 */
            if (EditorTitle.Text == "" || EditorDetails.Text == "") {
                str += "Empty Information!\n";
            }
            /* 检查日期是否合法 */
            if (EditorDate.Date < DateTime.Now.Date) {
                str += "Error Date!";
            }
            /* 对话框提醒 */
            if (str != "") {
                var i = new MessageDialog(str).ShowAsync();
                return false;
            }
            return true;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            Frame rootFrame = Window.Current.Content as Frame;

            if (e.Parameter.GetType() == typeof(ViewModels.TodoListItemViewModel)) {
                this.ViewModel = (ViewModels.TodoListItemViewModel)(e.Parameter);
            }
        }

        /* 添加按钮-新建页面 */
        private void AppBarAddClick(object sender, RoutedEventArgs e) {
            if (TodoItemEditor.Visibility == Visibility.Collapsed) {
                /* 传参为ViewModel */
                Frame.Navigate(typeof(NewPage), ViewModel);
            }
        }

        private void TodoListItemClick(object sender, ItemClickEventArgs e) {
            ViewModel.SelectedItem = (Models.TodoListItem)(e.ClickedItem);
            if (TodoItemEditor.Visibility == Visibility.Collapsed) {
                /* 传参为ViewModel */
                Frame.Navigate(typeof(NewPage), ViewModel);
            } else {
                TodoItemEditorInit();
            }
        }

        /* 创建按钮-点击事件 */
        private void TodoCreateClick(object sender, RoutedEventArgs e) {
            if (ItemContentJudge()) {
                /* 创建 */
                ViewModel.AddTodoItem(EditorTitle.Text, EditorDetails.Text, EditorDate.Date);
            }
        }

        /* 更新按钮-点击事件 */
        private void TodoUpdateClick(object sender, RoutedEventArgs e) {
            if (ItemContentJudge()) {
                /* 更新 */
                ViewModel.UpdateTodoItem(ViewModel.SelectedItem, EditorTitle.Text, EditorDetails.Text, EditorDate.Date);
                Frame.Navigate(typeof(MainPage), ViewModel); /* 刷新页面（更新ListItem的显示） */
            }
        }

        /* 取消按钮-点击事件 */
        private void TodoCancelClick(object sender, RoutedEventArgs e) {
            if (ViewModel.SelectedItem != null) {
                ViewModel.RemoveTodoItem(ViewModel.SelectedItem); /* 删除 */
            }
            TodoItemEditorInit(); /* 清空 */
        }

        /* 选择图片按钮-点击事件 */
        private async void SelectPictureClick(object sender, RoutedEventArgs e) {
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
                    EditorImage.Source = bitmapImage;
                }
            }
        }
    }
}
