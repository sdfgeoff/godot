/*************************************************************************/
/*  control.h                                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H

#include "core/math/transform_2d.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/templates/rid.h"
#include "scene/main/canvas_item.h"
#include "scene/main/timer.h"
#include "scene/resources/theme.h"

class Viewport;
class Label;
class Panel;

class Control : public CanvasItem {
	GDCLASS(Control, CanvasItem);

public:
	enum Anchor {
		ANCHOR_BEGIN = 0,
		ANCHOR_END = 1
	};

	enum GrowDirection {
		GROW_DIRECTION_BEGIN,
		GROW_DIRECTION_END,
		GROW_DIRECTION_BOTH
	};

	enum FocusMode {
		FOCUS_NONE,
		FOCUS_CLICK,
		FOCUS_ALL
	};

	enum SizeFlags {
		SIZE_SHRINK_BEGIN = 0,
		SIZE_FILL = 1,
		SIZE_EXPAND = 2,
		SIZE_SHRINK_CENTER = 4,
		SIZE_SHRINK_END = 8,

		SIZE_EXPAND_FILL = SIZE_EXPAND | SIZE_FILL,
	};

	enum MouseFilter {
		MOUSE_FILTER_STOP,
		MOUSE_FILTER_PASS,
		MOUSE_FILTER_IGNORE
	};

	enum CursorShape {
		CURSOR_ARROW,
		CURSOR_IBEAM,
		CURSOR_POINTING_HAND,
		CURSOR_CROSS,
		CURSOR_WAIT,
		CURSOR_BUSY,
		CURSOR_DRAG,
		CURSOR_CAN_DROP,
		CURSOR_FORBIDDEN,
		CURSOR_VSIZE,
		CURSOR_HSIZE,
		CURSOR_BDIAGSIZE,
		CURSOR_FDIAGSIZE,
		CURSOR_MOVE,
		CURSOR_VSPLIT,
		CURSOR_HSPLIT,
		CURSOR_HELP,
		CURSOR_MAX
	};

	enum LayoutPreset {
		PRESET_TOP_LEFT,
		PRESET_TOP_RIGHT,
		PRESET_BOTTOM_LEFT,
		PRESET_BOTTOM_RIGHT,
		PRESET_CENTER_LEFT,
		PRESET_CENTER_TOP,
		PRESET_CENTER_RIGHT,
		PRESET_CENTER_BOTTOM,
		PRESET_CENTER,
		PRESET_LEFT_WIDE,
		PRESET_TOP_WIDE,
		PRESET_RIGHT_WIDE,
		PRESET_BOTTOM_WIDE,
		PRESET_VCENTER_WIDE,
		PRESET_HCENTER_WIDE,
		PRESET_FULL_RECT
	};

	enum LayoutPresetMode {
		PRESET_MODE_MINSIZE,
		PRESET_MODE_KEEP_WIDTH,
		PRESET_MODE_KEEP_HEIGHT,
		PRESET_MODE_KEEP_SIZE
	};

	enum LayoutMode {
		LAYOUT_MODE_POSITION,
		LAYOUT_MODE_ANCHORS,
		LAYOUT_MODE_CONTAINER,
		LAYOUT_MODE_UNCONTROLLED,
	};

	enum LayoutDirection {
		LAYOUT_DIRECTION_INHERITED,
		LAYOUT_DIRECTION_LOCALE,
		LAYOUT_DIRECTION_LTR,
		LAYOUT_DIRECTION_RTL
	};

	enum TextDirection {
		TEXT_DIRECTION_AUTO = TextServer::DIRECTION_AUTO,
		TEXT_DIRECTION_LTR = TextServer::DIRECTION_LTR,
		TEXT_DIRECTION_RTL = TextServer::DIRECTION_RTL,
		TEXT_DIRECTION_INHERITED,
	};

private:
	struct CComparator {
		bool operator()(const Control *p_a, const Control *p_b) const {
			if (p_a->get_canvas_layer() == p_b->get_canvas_layer()) {
				return p_b->is_greater_than(p_a);
			}

			return p_a->get_canvas_layer() < p_b->get_canvas_layer();
		}
	};

	struct Data {
		// Global relations.

		List<Control *>::Element *RI = nullptr;

		Control *parent = nullptr;
		Window *parent_window = nullptr;
		CanvasItem *parent_canvas_item = nullptr;
		ObjectID drag_owner;

		// Positioning and sizing.

		real_t offset[4] = { 0.0, 0.0, 0.0, 0.0 };
		real_t anchor[4] = { ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN };
		FocusMode focus_mode = FOCUS_NONE;
		GrowDirection h_grow = GROW_DIRECTION_END;
		GrowDirection v_grow = GROW_DIRECTION_END;

		real_t rotation = 0.0;
		Vector2 scale = Vector2(1, 1);
		Vector2 pivot_offset;

		Point2 pos_cache;
		Size2 size_cache;
		Size2 minimum_size_cache;
		bool minimum_size_valid = false;

		Size2 last_minimum_size;
		bool updating_last_minimum_size = false;
		bool block_minimum_size_adjust = false;

		bool size_warning = true;

		// Container sizing.

		int h_size_flags = SIZE_FILL;
		int v_size_flags = SIZE_FILL;
		real_t expand = 1.0;
		Point2 custom_minimum_size;

		// Input events and rendering.

		MouseFilter mouse_filter = MOUSE_FILTER_STOP;
		bool force_pass_scroll_events = true;

		bool clip_contents = false;
		bool disable_visibility_clip = false;

		CursorShape default_cursor = CURSOR_ARROW;

		// Focus.

		NodePath focus_neighbor[4];
		NodePath focus_next;
		NodePath focus_prev;

		// Theming.

		Ref<Theme> theme;
		Control *theme_owner = nullptr;
		Window *theme_owner_window = nullptr;
		StringName theme_type_variation;

		bool bulk_theme_override = false;
		Theme::ThemeIconMap icon_override;
		Theme::ThemeStyleMap style_override;
		Theme::ThemeFontMap font_override;
		Theme::ThemeFontSizeMap font_size_override;
		Theme::ThemeColorMap color_override;
		Theme::ThemeConstantMap constant_override;

		// Internationalization.

		LayoutDirection layout_dir = LAYOUT_DIRECTION_INHERITED;
		bool is_rtl_dirty = true;
		bool is_rtl = false;

		bool auto_translate = true;

		// Extra properties.

		String tooltip;

	} data;

	// Dynamic properties.

	static constexpr unsigned properties_managed_by_container_count = 12;
	static String properties_managed_by_container[properties_managed_by_container_count];

	// Global relations.

	friend class Viewport;
	friend class Window;

	// Positioning and sizing.

	void _update_canvas_item_transform();
	Transform2D _get_internal_transform() const;

	void _set_anchor(Side p_side, real_t p_anchor);
	void _set_position(const Point2 &p_point);
	void _set_global_position(const Point2 &p_point);
	void _set_size(const Size2 &p_size);

	void _compute_offsets(Rect2 p_rect, const real_t p_anchors[4], real_t (&r_offsets)[4]);
	void _compute_anchors(Rect2 p_rect, const real_t p_offsets[4], real_t (&r_anchors)[4]);

	void _set_layout_mode(LayoutMode p_mode);
	LayoutMode _get_layout_mode() const;
	void _set_anchors_layout_preset(int p_preset);
	int _get_anchors_layout_preset() const;

	void _update_minimum_size_cache();
	void _update_minimum_size();
	void _size_changed();

	void _clear_size_warning();

	// Input events.

	void _call_gui_input(const Ref<InputEvent> &p_event);

	// Focus.

	void _window_find_focus_neighbor(const Vector2 &p_dir, Node *p_at, const Point2 *p_points, real_t p_min, real_t &r_closest_dist, Control **r_closest);
	Control *_get_focus_neighbor(Side p_side, int p_count = 0);

	// Theming.

	void _theme_changed();
	void _theme_property_override_changed();
	void _notify_theme_changed();

	static void _propagate_theme_changed(Node *p_at, Control *p_owner, Window *p_owner_window, bool p_assign = true);

	template <class T>
	static T get_theme_item_in_types(Control *p_theme_owner, Window *p_theme_owner_window, Theme::DataType p_data_type, const StringName &p_name, List<StringName> p_theme_types);
	static bool has_theme_item_in_types(Control *p_theme_owner, Window *p_theme_owner_window, Theme::DataType p_data_type, const StringName &p_name, List<StringName> p_theme_types);
	_FORCE_INLINE_ void _get_theme_type_dependencies(const StringName &p_theme_type, List<StringName> *p_list) const;

	// Extra properties.

	String _get_tooltip() const;

protected:
	// Dynamic properties.

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	virtual void _validate_property(PropertyInfo &property) const override;

	// Internationalization.

	virtual Array structured_text_parser(TextServer::StructuredTextParser p_parser_type, const Array &p_args, const String &p_text) const;

	// Base object overrides.

	virtual void add_child_notify(Node *p_child) override;
	virtual void remove_child_notify(Node *p_child) override;

	void _notification(int p_notification);
	static void _bind_methods();

	// Exposed virtual methods.

	GDVIRTUAL1RC(bool, _has_point, Vector2)
	GDVIRTUAL2RC(Array, _structured_text_parser, Array, String)
	GDVIRTUAL0RC(Vector2, _get_minimum_size)

	GDVIRTUAL1RC(Variant, _get_drag_data, Vector2)
	GDVIRTUAL2RC(bool, _can_drop_data, Vector2, Variant)
	GDVIRTUAL2(_drop_data, Vector2, Variant)
	GDVIRTUAL1RC(Object *, _make_custom_tooltip, String)

	GDVIRTUAL1(_gui_input, Ref<InputEvent>)

public:
	enum {
		NOTIFICATION_RESIZED = 40,
		NOTIFICATION_MOUSE_ENTER = 41,
		NOTIFICATION_MOUSE_EXIT = 42,
		NOTIFICATION_FOCUS_ENTER = 43,
		NOTIFICATION_FOCUS_EXIT = 44,
		NOTIFICATION_THEME_CHANGED = 45,
		NOTIFICATION_SCROLL_BEGIN = 47,
		NOTIFICATION_SCROLL_END = 48,
		NOTIFICATION_LAYOUT_DIRECTION_CHANGED = 49,
	};

	// Editor plugin interoperability.

	// TODO: Decouple controls from their editor plugin and get rid of this.
#ifdef TOOLS_ENABLED
	virtual Dictionary _edit_get_state() const override;
	virtual void _edit_set_state(const Dictionary &p_state) override;

	virtual void _edit_set_position(const Point2 &p_position) override;
	virtual Point2 _edit_get_position() const override;

	virtual void _edit_set_scale(const Size2 &p_scale) override;
	virtual Size2 _edit_get_scale() const override;

	virtual void _edit_set_rect(const Rect2 &p_edit_rect) override;
	virtual Rect2 _edit_get_rect() const override;
	virtual bool _edit_use_rect() const override;

	virtual void _edit_set_rotation(real_t p_rotation) override;
	virtual real_t _edit_get_rotation() const override;
	virtual bool _edit_use_rotation() const override;

	virtual void _edit_set_pivot(const Point2 &p_pivot) override;
	virtual Point2 _edit_get_pivot() const override;
	virtual bool _edit_use_pivot() const override;

	virtual Size2 _edit_get_minimum_size() const override;
#endif

	// Editor integration.

	virtual void get_argument_options(const StringName &p_function, int p_idx, List<String> *r_options) const override;
	TypedArray<String> get_configuration_warnings() const override;

	virtual bool is_text_field() const;

	// Global relations.

	bool is_top_level_control() const;

	Control *get_parent_control() const;
	Window *get_parent_window() const;
	Control *get_root_parent_control() const;

	Size2 get_parent_area_size() const;
	Rect2 get_parent_anchorable_rect() const;

	// Positioning and sizing.

	virtual Transform2D get_transform() const override;

	void set_anchor(Side p_side, real_t p_anchor, bool p_keep_offset = true, bool p_push_opposite_anchor = true);
	real_t get_anchor(Side p_side) const;
	void set_offset(Side p_side, real_t p_value);
	real_t get_offset(Side p_side) const;
	void set_anchor_and_offset(Side p_side, real_t p_anchor, real_t p_pos, bool p_push_opposite_anchor = true);

	// TODO: Rename to set_begin/end_offsets ?
	void set_begin(const Point2 &p_point);
	Point2 get_begin() const;
	void set_end(const Point2 &p_point);
	Point2 get_end() const;

	void set_h_grow_direction(GrowDirection p_direction);
	GrowDirection get_h_grow_direction() const;
	void set_v_grow_direction(GrowDirection p_direction);
	GrowDirection get_v_grow_direction() const;

	void set_anchors_preset(LayoutPreset p_preset, bool p_keep_offsets = true);
	void set_offsets_preset(LayoutPreset p_preset, LayoutPresetMode p_resize_mode = PRESET_MODE_MINSIZE, int p_margin = 0);
	void set_anchors_and_offsets_preset(LayoutPreset p_preset, LayoutPresetMode p_resize_mode = PRESET_MODE_MINSIZE, int p_margin = 0);
	void set_grow_direction_preset(LayoutPreset p_preset);

	void set_position(const Point2 &p_point, bool p_keep_offsets = false);
	void set_global_position(const Point2 &p_point, bool p_keep_offsets = false);
	Point2 get_position() const;
	Point2 get_global_position() const;
	Point2 get_screen_position() const;

	void set_size(const Size2 &p_size, bool p_keep_offsets = false);
	Size2 get_size() const;
	void reset_size();

	void set_rect(const Rect2 &p_rect); // Reset anchors to begin and set rect, for faster container children sorting.
	Rect2 get_rect() const;
	Rect2 get_global_rect() const;
	Rect2 get_screen_rect() const;
	Rect2 get_window_rect() const; ///< use with care, as it blocks waiting for the rendering server
	Rect2 get_anchorable_rect() const override;

	void set_scale(const Vector2 &p_scale);
	Vector2 get_scale() const;
	void set_rotation(real_t p_radians);
	real_t get_rotation() const;
	void set_pivot_offset(const Vector2 &p_pivot);
	Vector2 get_pivot_offset() const;

	void update_minimum_size();

	void set_block_minimum_size_adjust(bool p_block);
	bool is_minimum_size_adjust_blocked() const;

	virtual Size2 get_minimum_size() const;
	virtual Size2 get_combined_minimum_size() const;

	void set_custom_minimum_size(const Size2 &p_custom);
	Size2 get_custom_minimum_size() const;

	// Container sizing.

	void set_h_size_flags(int p_flags);
	int get_h_size_flags() const;
	void set_v_size_flags(int p_flags);
	int get_v_size_flags() const;
	void set_stretch_ratio(real_t p_ratio);
	real_t get_stretch_ratio() const;

	// Input events.

	virtual void gui_input(const Ref<InputEvent> &p_event);
	void accept_event();

	virtual bool has_point(const Point2 &p_point) const;

	void set_mouse_filter(MouseFilter p_filter);
	MouseFilter get_mouse_filter() const;

	void set_force_pass_scroll_events(bool p_force_pass_scroll_events);
	bool is_force_pass_scroll_events() const;

	void warp_mouse(const Point2 &p_position);

	// Drag and drop handling.

	virtual void set_drag_forwarding(Object *p_target);
	virtual Variant get_drag_data(const Point2 &p_point);
	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data);
	void set_drag_preview(Control *p_control);
	void force_drag(const Variant &p_data, Control *p_control);
	bool is_drag_successful() const;

	// Focus.

	void set_focus_mode(FocusMode p_focus_mode);
	FocusMode get_focus_mode() const;
	bool has_focus() const;
	void grab_focus();
	void grab_click_focus();
	void release_focus();

	Control *find_next_valid_focus() const;
	Control *find_prev_valid_focus() const;

	void set_focus_neighbor(Side p_side, const NodePath &p_neighbor);
	NodePath get_focus_neighbor(Side p_side) const;

	void set_focus_next(const NodePath &p_next);
	NodePath get_focus_next() const;
	void set_focus_previous(const NodePath &p_prev);
	NodePath get_focus_previous() const;

	// Rendering.

	void set_default_cursor_shape(CursorShape p_shape);
	CursorShape get_default_cursor_shape() const;
	virtual CursorShape get_cursor_shape(const Point2 &p_pos = Point2i()) const;

	void set_clip_contents(bool p_clip);
	bool is_clipping_contents();

	void set_disable_visibility_clip(bool p_ignore);
	bool is_visibility_clip_disabled() const;

	// Theming.

	void set_theme(const Ref<Theme> &p_theme);
	Ref<Theme> get_theme() const;

	void set_theme_type_variation(const StringName &p_theme_type);
	StringName get_theme_type_variation() const;

	void begin_bulk_theme_override();
	void end_bulk_theme_override();

	void add_theme_icon_override(const StringName &p_name, const Ref<Texture2D> &p_icon);
	void add_theme_style_override(const StringName &p_name, const Ref<StyleBox> &p_style);
	void add_theme_font_override(const StringName &p_name, const Ref<Font> &p_font);
	void add_theme_font_size_override(const StringName &p_name, int p_font_size);
	void add_theme_color_override(const StringName &p_name, const Color &p_color);
	void add_theme_constant_override(const StringName &p_name, int p_constant);

	void remove_theme_icon_override(const StringName &p_name);
	void remove_theme_style_override(const StringName &p_name);
	void remove_theme_font_override(const StringName &p_name);
	void remove_theme_font_size_override(const StringName &p_name);
	void remove_theme_color_override(const StringName &p_name);
	void remove_theme_constant_override(const StringName &p_name);

	Ref<Texture2D> get_theme_icon(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	Ref<StyleBox> get_theme_stylebox(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	Ref<Font> get_theme_font(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	int get_theme_font_size(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	Color get_theme_color(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	int get_theme_constant(const StringName &p_name, const StringName &p_theme_type = StringName()) const;

	bool has_theme_icon_override(const StringName &p_name) const;
	bool has_theme_stylebox_override(const StringName &p_name) const;
	bool has_theme_font_override(const StringName &p_name) const;
	bool has_theme_font_size_override(const StringName &p_name) const;
	bool has_theme_color_override(const StringName &p_name) const;
	bool has_theme_constant_override(const StringName &p_name) const;

	bool has_theme_icon(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	bool has_theme_stylebox(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	bool has_theme_font(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	bool has_theme_font_size(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	bool has_theme_color(const StringName &p_name, const StringName &p_theme_type = StringName()) const;
	bool has_theme_constant(const StringName &p_name, const StringName &p_theme_type = StringName()) const;

	static float fetch_theme_default_base_scale(Control *p_theme_owner, Window *p_theme_owner_window);
	static Ref<Font> fetch_theme_default_font(Control *p_theme_owner, Window *p_theme_owner_window);
	static int fetch_theme_default_font_size(Control *p_theme_owner, Window *p_theme_owner_window);

	float get_theme_default_base_scale() const;
	Ref<Font> get_theme_default_font() const;
	int get_theme_default_font_size() const;

	// Internationalization.

	void set_layout_direction(LayoutDirection p_direction);
	LayoutDirection get_layout_direction() const;
	virtual bool is_layout_rtl() const;

	void set_auto_translate(bool p_enable);
	bool is_auto_translating() const;
	_FORCE_INLINE_ String atr(const String p_string) const {
		return is_auto_translating() ? tr(p_string) : p_string;
	};

	// Extra properties.

	void set_tooltip(const String &p_tooltip);
	virtual String get_tooltip(const Point2 &p_pos) const;
	virtual Control *make_custom_tooltip(const String &p_text) const;

	Control() {}
};

VARIANT_ENUM_CAST(Control::FocusMode);
VARIANT_ENUM_CAST(Control::SizeFlags);
VARIANT_ENUM_CAST(Control::CursorShape);
VARIANT_ENUM_CAST(Control::LayoutPreset);
VARIANT_ENUM_CAST(Control::LayoutPresetMode);
VARIANT_ENUM_CAST(Control::MouseFilter);
VARIANT_ENUM_CAST(Control::GrowDirection);
VARIANT_ENUM_CAST(Control::Anchor);
VARIANT_ENUM_CAST(Control::LayoutMode);
VARIANT_ENUM_CAST(Control::LayoutDirection);
VARIANT_ENUM_CAST(Control::TextDirection);

#endif // CONTROL_H
