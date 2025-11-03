#pragma once
#include <memory>       // 用于 std::unique_ptr
#include <stdexcept>    // 用于 std::runtime_error
#include <string>       // 用于 std::string
#include <string_view>  // 用于 std::string_view
#include <unordered_map>// 用于 std::unordered_map
#include <utility>      // 用于 std::pair
#include <functional>   // 用于 std::hash
#include <SDL_ttf.h>    // SDL_ttf 主头文件

namespace engine::resource 
{
// 定义字体键类型（路径 + 大小）
using FontKey = std::pair<std::string, int>;

// FontKey 的自定义哈希函数,用于 std::unordered_map
struct FontKeyHash {
    std::size_t operator()(const FontKey& key)const
    {
        std::hash<std::string> string_hasher;
        std::hash<int> int_hasher;
        return string_hasher(key.first) ^ int_hasher(key.second); // 异或运算
    }
};

//管理 SDL_ttf 字体资源（TTF_Font）。
//提供字体的加载和缓存功能，通过文件路径和点大小来标识。
//仅供 ResourceManager 内部使用。
class FontManager final
{
    friend class ResourceManager;
private:
    // TTF_Font 的自定义删除器
    struct SDLFontDeleter 
    {
        void operator()(TTF_Font* font)const
        {
            if (font)
                TTF_CloseFont(font);
        }
    };
    // 字体存储（FontKey -> TTF_Font）。  
    // 对于自定义类型，需要提供自定义哈希函数（第三个模版参数）
    std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> fonts_;

public:
    FontManager() = default;
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(FontManager&&) = delete;
    
    ~FontManager() = default;
private: // 仅由 ResourceManager（和内部）访问的方法
    
    TTF_Font* loadFont(std::string_view file_path, int point_size);      //从文件路径加载指定点大小的字体
    TTF_Font* getFont(std::string_view file_path, int point_size);       //尝试获取已加载字体的指针，如果未加载则尝试加载
    void unloadFont(std::string_view file_path, int point_size);         //卸载特定字体（通过路径和大小标识）
    void clear();                                                        //清空所有缓存的字体
};
} // namespace engine::resource
