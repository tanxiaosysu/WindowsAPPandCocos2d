using NotificationsExtensions.Tiles;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.DataTransfer;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

//“空白页”项模板在 http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409 上有介绍

namespace HW05 {
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();
            this.ViewModel = new ViewModels.TodoListItemViewModel();

            DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();
            dataTransferManager.DataRequested +=
                new TypedEventHandler<DataTransferManager,
                    DataRequestedEventArgs>(this.ShareDataHandler);
        }

        private ViewModels.TodoListItemViewModel ViewModel;

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            Frame rootFrame = Window.Current.Content as Frame;

            if (e.Parameter.GetType() == typeof(ViewModels.TodoListItemViewModel)) {
                this.ViewModel = (ViewModels.TodoListItemViewModel)(e.Parameter);
            }
        }

        /* 数据共享处理 */
        private void ShareDataHandler(DataTransferManager sender, DataRequestedEventArgs e) {
            DataPackage dp = e.Request.Data;
            // The Title is mandatory
            /* 设置标题 */
            dp.Properties.Title = ViewModel.SelectedItem.title;
            // Now add the data you want to share.
            /* 设置描述 */
            dp.Properties.Description = "Content of A Todo Item";
            /* 设置内容 */
            dp.SetText(ViewModel.SelectedItem.description);
        }

        /* 添加按钮-新建页面 */
        private void AppBarAddClick(object sender, RoutedEventArgs e) {
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        /* ListItem点击事件 */
        private void TodoListItemClick(object sender, ItemClickEventArgs e) {
            ViewModel.SelectedItem = (Models.TodoListItem)(e.ClickedItem);
            Frame.Navigate(typeof(NewPage), ViewModel);
        }

        /* 更新磁贴 */
        private void UpdateTileClick(object sender, RoutedEventArgs e) {
            /* 获取Item */
            Models.TodoListItem tempItem = ViewModel.AllItems[0];
            /* 创建TileContent */
            TileContent content = new TileContent() {
                Visual = new TileVisual() {
                    TileSmall = new TileBinding() {
                        Content = new TileBindingContentAdaptive() {
                            Children = {
                                new TileText() {
                                    Text = tempItem.title,
                                    Style=TileTextStyle.Base
                                }
                            }
                        }
                    },

                    TileMedium = new TileBinding() {
                        Content = new TileBindingContentAdaptive() {
                            Children = {
                                new TileText() {
                                    Text = tempItem.title,
                                    Style=TileTextStyle.Base
                                },
                                new TileText() {
                                    Text = "details:" + tempItem.description,
                                    Style = TileTextStyle.CaptionSubtle,
                                    Wrap = true
                                }
                            }
                        }
                    },

                    TileWide = new TileBinding() {
                        Content = new TileBindingContentAdaptive() {
                            Children = {
                                new TileText() {
                                    Text = "title:" + tempItem.title,
                                    Style=TileTextStyle.Base
                                },
                                new TileText() {
                                    Text = "details:" + tempItem.description,
                                    Style = TileTextStyle.CaptionSubtle,
                                    Wrap = true
                                }
                            }
                        }
                    }
                }
            };
            var notification = new TileNotification(content.GetXml());
            var updater = TileUpdateManager.CreateTileUpdaterForApplication();
            /* 更新 */
            updater.Update(notification);
        }

        /* 共享 */
        private void ItemShareClick(object sender, RoutedEventArgs e) {
            /* 设置SelectedItem来传递参数给DataHandler */
            ViewModel.SelectedItem = (Models.TodoListItem)(((MenuFlyoutItem)e.OriginalSource).DataContext);
            DataTransferManager.ShowShareUI();
        }
    }
}
