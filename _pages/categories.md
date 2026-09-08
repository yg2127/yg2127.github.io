---
layout: content
title: Stack
permalink: /stack/
---

<div class="c-stack__grid">
{% assign sorted_categories = site.categories | sort %}
{% for category in sorted_categories %}
{% assign cat_name = category[0] %}
{% assign slug = cat_name | downcase | replace: ' ', '-' %}
{% assign icon = "" %}
{% if cat_name == "Python" %}{% assign icon = "devicon-python-plain colored" %}
{% elsif cat_name == "Pandas & Plotly" %}{% assign icon = "devicon-pandas-plain colored" %}
{% elsif cat_name == "Streamlit" %}{% assign icon = "devicon-streamlit-plain colored" %}
{% elsif cat_name == "SQL" %}{% assign icon = "devicon-azuresqldatabase-plain colored" %}
{% elsif cat_name == "Deep Learning" %}{% assign icon = "devicon-pytorch-original colored" %}
{% elsif cat_name == "Git" %}{% assign icon = "devicon-git-plain colored" %}
{% endif %}
  <a class="c-stack__card" href="{{ '/stack/' | append: slug | append: '/' | prepend: site.baseurl }}">
    <span class="c-stack__icon">{% if icon != "" %}<i class="{{ icon }}"></i>{% else %}<i class="devicon-devicon-plain"></i>{% endif %}</span>
    <span class="c-stack__name">{{ cat_name }}</span>
    <span class="c-stack__count">{{ category[1] | size }} posts</span>
  </a>
{% endfor %}
</div>
