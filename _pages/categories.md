---
layout: content
title: Stack
permalink: /stack/
---

<p class="c-stack__intro">기술 스택별로 글을 모아두었습니다. 카드를 클릭하면 해당 스택의 글 목록으로 이동합니다.</p>

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
{% endif %}
  <a class="c-stack__card" href="{{ '/stack/' | append: slug | append: '/' | prepend: site.baseurl }}">
    <span class="c-stack__icon">{% if icon != "" %}<i class="{{ icon }}"></i>{% else %}<i class="devicon-devicon-plain"></i>{% endif %}</span>
    <span class="c-stack__name">{{ cat_name }}</span>
    <span class="c-stack__count">{{ category[1] | size }} posts</span>
  </a>
{% endfor %}
</div>
